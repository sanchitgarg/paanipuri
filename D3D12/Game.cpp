//
// Game.cpp
//

#include "pch.h"
#include "Game.h"
#include <random>
#include <algorithm>
#include <iterator>
#include <glm/glm.hpp>

extern void ExitGame();

using namespace DirectX;
using namespace DirectX::SimpleMath;

using Microsoft::WRL::ComPtr;

static int numVertices = 0;
glm::vec3 camEye = glm::vec3(0, 7, 150);
glm::vec3 camDir = glm::vec3(0, -5, 0);
glm::vec3 camUp = glm::vec3(0, -1, 0);
float yaw1 = -90.0f;
float pitch1 = 0.0f;


Game::Game() noexcept(false)
{
    m_deviceResources = std::make_unique<DX::DeviceResources>();
    m_deviceResources->RegisterDeviceNotify(this);
}

// Initialize the Direct3D resources required to run.
void Game::Initialize(IUnknown* window, int width, int height, DXGI_MODE_ROTATION rotation)
{
    m_gamePad = std::make_unique<GamePad>();

    m_keyboard = std::make_unique<Keyboard>();
    m_keyboard->SetWindow(reinterpret_cast<ABI::Windows::UI::Core::ICoreWindow*>(window));

    m_mouse = std::make_unique<Mouse>();
    m_mouse->SetWindow(reinterpret_cast<ABI::Windows::UI::Core::ICoreWindow*>(window));

    m_deviceResources->SetWindow(window, width, height, rotation);

    m_deviceResources->CreateDeviceResources();
    CreateDeviceDependentResources();

    m_deviceResources->CreateWindowSizeDependentResources();
    CreateWindowSizeDependentResources();

    // Create DirectXTK for Audio objects
    AUDIO_ENGINE_FLAGS eflags = AudioEngine_Default;
#ifdef _DEBUG
    eflags = eflags | AudioEngine_Debug;
#endif

    m_audEngine = std::make_unique<AudioEngine>(eflags);

    m_audioEvent = 0;
    m_audioTimerAcc = 10.f;
    m_retryDefault = false;

    m_waveBank = std::make_unique<WaveBank>(m_audEngine.get(), L"assets\\adpcmdroid.xwb");
    m_soundEffect = std::make_unique<SoundEffect>(m_audEngine.get(), L"assets\\MusicMono_adpcm.wav");
    m_effect1 = m_soundEffect->CreateInstance();
    m_effect2 = m_waveBank->CreateInstance(10);

    m_effect1->Play(true);
    m_effect2->Play();
}

#pragma region Frame Update
// Executes the basic game loop.
void Game::Tick()
{
    m_timer.Tick([&]()
    {
        Update(m_timer);
    });

    Render();
}

// Updates the world.
void Game::Update(DX::StepTimer const& timer)
{
    PIXBeginEvent(PIX_COLOR_DEFAULT, L"Update");

    // todo sangarg : These are the defaults from the project. The camera setup might need to be changed.
    // Vector3 eye = Vector3(0.0f, 7.0f, 150.0f);
    //Vector3 camDir = Vector3(0.0f, -5.0f, 0.0f);
    
    Vector3 eye(0.0f, 0.7f, 1.5f);
    Vector3 at(0.0f, -0.1f, 0.0f);

    m_view = Matrix::CreateLookAt(eye, at, Vector3::UnitY);

    m_world = Matrix::CreateRotationY(float(timer.GetTotalSeconds() * XM_PIDIV4));

    m_lineEffect->SetView(m_view);
    m_lineEffect->SetWorld(Matrix::Identity);

    m_shapeEffect->SetView(m_view);

    m_audioTimerAcc -= (float)timer.GetElapsedSeconds();
    if (m_audioTimerAcc < 0)
    {
        if (m_retryDefault)
        {
            m_retryDefault = false;
            if (m_audEngine->Reset())
            {
                // Restart looping audio
                m_effect1->Play(true);
            }
        }
        else
        {
            m_audioTimerAcc = 4.f;

            m_waveBank->Play(m_audioEvent++);

            if (m_audioEvent >= 11)
                m_audioEvent = 0;
        }
    }

    auto pad = m_gamePad->GetState(0);
    if (pad.IsConnected())
    {
        m_gamePadButtons.Update(pad);

        if (pad.IsViewPressed())
        {
            ExitGame();
        }
    }
    else
    {
        m_gamePadButtons.Reset();
    }

    auto kb = m_keyboard->GetState();
    m_keyboardButtons.Update(kb);

    if (kb.Escape)
    {
        ExitGame();
    }

    auto mouse = m_mouse->GetState();
    mouse;

    PIXEndEvent();
}
#pragma endregion

#pragma region Frame Render
// Draws the scene.
void Game::Render()
{
    // Don't try to render anything before the first Update.
    if (m_timer.GetFrameCount() == 0)
    {
        return;
    }

    // Prepare the command list to render a new frame.
    m_deviceResources->Prepare();
    Clear();

    auto commandList = m_deviceResources->GetCommandList();
    PIXBeginEvent(commandList, PIX_COLOR_DEFAULT, L"Render");

    // Draw procedurally generated dynamic grid
    const XMVECTORF32 xaxis = { 20.f, 0.f, 0.f };
    const XMVECTORF32 yaxis = { 0.f, 0.f, 20.f };
    DrawGrid(xaxis, yaxis, g_XMZero, 100, 100, Colors::White);

    // Set the descriptor heaps
    ID3D12DescriptorHeap* heaps[] = { m_resourceDescriptors->Heap(), m_states->Heap() };
    commandList->SetDescriptorHeaps(_countof(heaps), heaps);

    // Show the new frame.
    PIXBeginEvent(m_deviceResources->GetCommandQueue(), PIX_COLOR_DEFAULT, L"Present");
    m_deviceResources->Present();
    m_graphicsMemory->Commit(m_deviceResources->GetCommandQueue());
    PIXEndEvent(m_deviceResources->GetCommandQueue());
}

// Helper method to clear the back buffers.
void Game::Clear()
{
    auto commandList = m_deviceResources->GetCommandList();
    PIXBeginEvent(commandList, PIX_COLOR_DEFAULT, L"Clear");

    // Clear the views.
    auto rtvDescriptor = m_deviceResources->GetRenderTargetView();
    auto dsvDescriptor = m_deviceResources->GetDepthStencilView();

    commandList->OMSetRenderTargets(1, &rtvDescriptor, FALSE, &dsvDescriptor);
    commandList->ClearRenderTargetView(rtvDescriptor, Colors::Black, 0, nullptr);
    commandList->ClearDepthStencilView(dsvDescriptor, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

    // Set the viewport and scissor rect.
    auto viewport = m_deviceResources->GetScreenViewport();
    auto scissorRect = m_deviceResources->GetScissorRect();
    commandList->RSSetViewports(1, &viewport);
    commandList->RSSetScissorRects(1, &scissorRect);

    PIXEndEvent(commandList);
}

void XM_CALLCONV Game::DrawGrid(FXMVECTOR xAxis, FXMVECTOR yAxis, FXMVECTOR origin, size_t xdivs, size_t ydivs, GXMVECTOR color)
{
    auto commandList = m_deviceResources->GetCommandList();
    PIXBeginEvent(commandList, PIX_COLOR_DEFAULT, L"Draw grid");

    m_lineEffect->Apply(commandList);

    m_batch->Begin(commandList);

    int gridSize = 5;
    // std::random_device rd;
    // std::mt19937 g(rd());

    // std::uniform_int_distribution<> dis(1, gridSize*gridSize*gridSize);

    std::vector<VertexPositionColor> vertices;

    int count = 0;
    float jump = 1.0f;
    // int randomSize = dis(g);

    float x = 0.0f, y = 0.0f, z = 0.0f;

    for (x = -jump * gridSize; x <= jump * gridSize; x += jump)
    {
        for (y = -jump * gridSize; y <= jump * gridSize; y += jump)
        {
            for (z = -jump * gridSize; z <= jump * gridSize; z += jump)
            {
                XMVECTORF32 v = { x, y, z };
                vertices.push_back(VertexPositionColor(XMVectorAdd(origin, v), color));
                ++count;
            }
        }
    }

    // std::shuffle(vertices.begin(), vertices.end(), g);

    // std::vector<VertexPositionColor> v2;
    // v2.reserve(randomSize);
    // v2.assign(vertices.begin(), vertices.begin() + randomSize);

    m_batch->DrawPoints(vertices);

    m_batch->End();

    PIXEndEvent(commandList);
}

void XM_CALLCONV Game::DrawPoint(FXMVECTOR position, GXMVECTOR color)
{
    auto commandList = m_deviceResources->GetCommandList();
    PIXBeginEvent(commandList, PIX_COLOR_DEFAULT, L"Draw point");

    m_points->Begin(commandList);

    // const XMVECTORF32 point = { 0.f, 0.f, 0.f };
    VertexPositionColor vPoint (position, color);

    // m_points->DrawPoint(vPoint);
    m_points->End();

    PIXEndEvent(commandList);
}

#pragma endregion

#pragma region Message Handlers
// Message handlers
void Game::OnActivated()
{
}

void Game::OnDeactivated()
{
}

void Game::OnSuspending()
{
    m_audEngine->Suspend();
}

void Game::OnResuming()
{
    m_timer.ResetElapsedTime();
    m_gamePadButtons.Reset();
    m_keyboardButtons.Reset();
    m_audEngine->Resume();
}

void Game::OnWindowSizeChanged(int width, int height, DXGI_MODE_ROTATION rotation)
{
    if (!m_deviceResources->WindowSizeChanged(width, height, rotation))
        return;

    CreateWindowSizeDependentResources();
}

void Game::ValidateDevice()
{
    m_deviceResources->ValidateDevice();
}

void Game::NewAudioDevice()
{
    if (m_audEngine && !m_audEngine->IsAudioDevicePresent())
    {
        // Setup a retry in 1 second
        m_audioTimerAcc = 1.f;
        m_retryDefault = true;
    }
}

// Properties
void Game::GetDefaultSize(int& width, int& height) const
{
    width = 800;
    height = 600;
}
#pragma endregion

#pragma region Direct3D Resources
// These are the resources that depend on the device.
void Game::CreateDeviceDependentResources()
{
    auto device = m_deviceResources->GetD3DDevice();

    m_graphicsMemory = std::make_unique<GraphicsMemory>(device);

    m_states = std::make_unique<CommonStates>(device);

    m_resourceDescriptors = std::make_unique<DescriptorHeap>(device,
        D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
        D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE,
        Descriptors::Count);

    m_batch = std::make_unique<PrimitiveBatch<VertexPositionColor>>(device);
    m_points = std::make_unique<PrimitiveBatch<VertexPositionColor>>(device);

    m_shape = GeometricPrimitive::CreateTeapot(4.f, 8);

    // SDKMESH has to use clockwise winding with right-handed coordinates, so textures are flipped in U
    m_model = Model::CreateFromSDKMESH(L"assets\\tiny.sdkmesh");

    {
        ResourceUploadBatch resourceUpload(device);

        resourceUpload.Begin();

        m_model->LoadStaticBuffers(device, resourceUpload);

        DX::ThrowIfFailed(
            CreateDDSTextureFromFile(device, resourceUpload, L"assets\\seafloor.dds", m_texture1.ReleaseAndGetAddressOf())
        );

        CreateShaderResourceView(device, m_texture1.Get(), m_resourceDescriptors->GetCpuHandle(Descriptors::SeaFloor));

        DX::ThrowIfFailed(
            CreateDDSTextureFromFile(device, resourceUpload, L"assets\\windowslogo.dds", m_texture2.ReleaseAndGetAddressOf())
        );

        CreateShaderResourceView(device, m_texture2.Get(), m_resourceDescriptors->GetCpuHandle(Descriptors::WindowsLogo));

        RenderTargetState rtState(m_deviceResources->GetBackBufferFormat(), m_deviceResources->GetDepthBufferFormat());

        {
            EffectPipelineStateDescription pd(
                &VertexPositionColor::InputLayout,
                CommonStates::Opaque,
                CommonStates::DepthNone,
                CommonStates::CullNone,
                rtState,
                D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT);

            m_lineEffect = std::make_unique<BasicEffect>(device, EffectFlags::VertexColor, pd);
        }

        {
            EffectPipelineStateDescription pd(
                &GeometricPrimitive::VertexType::InputLayout,
                CommonStates::Opaque,
                CommonStates::DepthDefault,
                CommonStates::CullNone,
                rtState);

            m_shapeEffect = std::make_unique<BasicEffect>(device, EffectFlags::PerPixelLighting | EffectFlags::Texture, pd);
            m_shapeEffect->EnableDefaultLighting();
            m_shapeEffect->SetTexture(m_resourceDescriptors->GetGpuHandle(Descriptors::SeaFloor), m_states->LinearWrap());
        }

        {
            SpriteBatchPipelineStateDescription pd(rtState);

            m_sprites = std::make_unique<SpriteBatch>(device, resourceUpload, pd);
        }

        m_modelResources = m_model->LoadTextures(device, resourceUpload, L"assets\\");

        {
            EffectPipelineStateDescription psd(
                nullptr,
                CommonStates::Opaque,
                CommonStates::DepthDefault,
                CommonStates::CullNone,
                rtState);

            m_modelEffects = m_model->CreateEffects(psd, psd, m_modelResources->Heap(), m_states->Heap());
        }

        m_font = std::make_unique<SpriteFont>(device, resourceUpload,
            L"assets\\SegoeUI_18.spritefont",
            m_resourceDescriptors->GetCpuHandle(Descriptors::SegoeFont),
            m_resourceDescriptors->GetGpuHandle(Descriptors::SegoeFont));

        // Upload the resources to the GPU.
        auto uploadResourcesFinished = resourceUpload.End(m_deviceResources->GetCommandQueue());

        // Wait for the command list to finish executing
        m_deviceResources->WaitForGpu();

        // Wait for the upload thread to terminate
        uploadResourcesFinished.wait();
    }
}

// Allocate all memory resources that change on a window SizeChanged event.
void Game::CreateWindowSizeDependentResources()
{
    auto size = m_deviceResources->GetOutputSize();
    float aspectRatio = float(size.right) / float(size.bottom);
    float fovAngleY = 70.0f * XM_PI / 180.0f;

    // This is a simple example of change that can be made when the app is in
    // portrait or snapped view.
    if (aspectRatio < 1.0f)
    {
        fovAngleY *= 2.0f;
    }

    // Note that the OrientationTransform3D matrix is post-multiplied here
    // in order to correctly orient the scene to match the display orientation.
    // This post-multiplication step is required for any draw calls that are
    // made to the swap chain render target. For draw calls to other targets,
    // this transform should not be applied.

    // This sample makes use of a right-handed coordinate system using row-major matrices.
    Matrix perspectiveMatrix = Matrix::CreatePerspectiveFieldOfView(
        fovAngleY,
        aspectRatio,
        0.01f,
        100.0f
    );

    Matrix orientationMatrix = m_deviceResources->GetOrientationTransform3D();

    m_projection = perspectiveMatrix * orientationMatrix;

    m_lineEffect->SetProjection(m_projection);
    m_shapeEffect->SetProjection(m_projection);

    auto viewport = m_deviceResources->GetScreenViewport();
    m_sprites->SetViewport(viewport);

    m_sprites->SetRotation(m_deviceResources->GetRotation());
}

void Game::OnDeviceLost()
{
    m_texture1.Reset();
    m_texture2.Reset();

    m_font.reset();
    m_batch.reset();
    m_points.reset();
    m_shape.reset();
    m_model.reset();
    m_lineEffect.reset();
    m_shapeEffect.reset();
    m_modelEffects.clear();
    m_modelResources.reset();
    m_sprites.reset();
    m_resourceDescriptors.reset();
    m_states.reset();
    m_graphicsMemory.reset();
}

void Game::OnDeviceRestored()
{
    CreateDeviceDependentResources();

    CreateWindowSizeDependentResources();
}
#pragma endregion
