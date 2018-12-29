//
//  scene.cpp
//  paani
//
//  Created by Sanchit Garg on 3/12/15.
//  Copyright (c) 2015 Debanshu. All rights reserved.
//

#include "pch.h"
#include "scene.h"
#include <iostream>
#include "Mesh.h"

using namespace std;

int currPhaseBall = 2;
int currPhaseCube = 3;

//Cube class functions
Cube::Cube(glm::vec3 c, glm::vec3 d)
{
    center = c;
    dimensions = d;
}

glm::vec3 Cube::getCenter()
{
    return center;
}

glm::vec3 Cube::getDimensions()
{
    return dimensions;
}

glm::vec3 Cube::getHalfDimensions()
{
    return dimensions/2.0f;
}

float Cube::getCellSize()
{
    return cellSize;
}

void Cube::setCenter(glm::vec3 c)
{
    center = c;
}

void Cube::setDimension(glm::vec3 d)
{
    dimensions = d;
}

void Cube::setCellSize(float s)
{
    cellSize = s;
}

Scene::Scene()
{
    //need to add particles
    // create box
    
    gravity = glm::vec3(0.0,-10.0,0.0);
    
    numberOfParticles = 0;
    
    particleSystem = new ParticleSystem();
    
    cube = new Cube();
    cube->setCenter(glm::vec3(0,0,0));
    cube->setDimension(glm::vec3(20)*particleSystem->getSmoothingRadius());
    cube->setCellSize(particleSystem->getSmoothingRadius());       //depends on cube dimensions and particle radius
    
}

void Scene::init(){
    glm::vec3 position(0), velocity(0);
    
//    float smoothingRad = particleSystem->getSmoothingRadius() * 1.f;
//    int damDim = static_cast <int> (std::cbrt(numberOfParticles)),
//    i,j,k=0;
//    
//    //set up dam break
//    
//    //  start with the highest y, and keep filling squares
//    
//    int phase = 0;
//    float mass = 1.f;
//    for(i=0; i<damDim; i++)
//    {
//        for(j=0; j<damDim; j++)
//        {
//            for(k=0; k<damDim; k++)
//            {
//                position = (glm::vec3(i,j,k)*smoothingRad - glm::vec3(float(damDim) * smoothingRad/2.0f))*0.9f;
//                particleSystem->addParticle(Particle(position, velocity, phase, mass));
//            }
//        }
//    }

//    addBallToScene();
//    addParticlesToScene(1);
//    addCubeToScene();


    particleSystem->setUpperBounds(cube->getCenter() + cube->getHalfDimensions());
    particleSystem->setLowerBounds(cube->getCenter() - cube->getHalfDimensions());
    particleSystem->setCellSize(cube->getCellSize());
    particleSystem->setForces(gravity);
    
    std::string objPath = "GlassBowl.obj";
    mesh.LoadMesh(objPath);
    
    particleSystem->loadContainer(mesh);
}

void Scene::addParticlesToScene(int type)
{
    int particleCount = 1000;
    
    int damDim = static_cast <int> (std::cbrt(particleCount)),
        i,j,k=0;
    float smoothingRad = particleSystem->getSmoothingRadius() * 1.f;
    glm::vec3 position(0), velocity(0);
    
    int phase = 1;
    float mass = 4.f;
    
    switch (type) {

        case 1:
            phase = 0;
            mass = 10.f;
            break;
            
        case 2:
            phase = 1;
            mass = 1.f;
            break;
            
        default:
            break;
    }
    
    for(i=0; i<damDim; i++)
    {
        for(j=0; j<damDim; j++)
        {
            for(k=0; k<damDim; k++)
            {
                position = (glm::vec3(i,j,k)*smoothingRad - glm::vec3(float(damDim) * smoothingRad/2.0f))*0.9f;
                particleSystem->addParticle(Particle(position, velocity, phase, mass));
//                particleSystem->particleSizData.push_back(glm::vec3(2.0f));
            }
        }
    }
    
    numberOfParticles += particleCount;
}

void Scene::pourFluid(int type)
{
    int particleCount = 27;
    int damDim = static_cast <int> (std::cbrt(particleCount)),
    i,j,k=0;
    float smoothingRad = particleSystem->getSmoothingRadius() * 1.f;
    glm::vec3 position(0), translate(0), velocity(10,0,0);
    
    int phase = 0;
    float mass = 10.f;
    
    switch (type) {
            
        case 1:
            phase = 0;
            mass = 10.f;
            translate = glm::vec3(-cube->getHalfDimensions().x*0.5,cube->getHalfDimensions().y*0.9,0);
            velocity = glm::vec3(10,0,0);
            break;
            
        case 2:
            phase = 1;
            mass = 1.f;
            translate = glm::vec3(cube->getHalfDimensions().x*0.5,cube->getHalfDimensions().y*0.9,0);
            velocity = glm::vec3(-10,0,0);
            break;
            
        default:
            break;
    }
    
    for(i=0; i<damDim; i++)
    {
        for(j=0; j<damDim; j++)
        {
            for(k=0; k<damDim; k++)
            {
                position = (glm::vec3(i,j,k)*smoothingRad - glm::vec3(float(damDim) * smoothingRad/2.0f))*0.5f + translate;
                particleSystem->addParticle(Particle(position, velocity, phase, mass));
//                particleSystem->particleSizData.push_back(glm::vec3(2.0f));
            }
        }
    }
    
    numberOfParticles += particleCount;
}

void Scene::addCubeToScene()
{
    int particleCount = 27;

    int damDim = static_cast <int> (std::cbrt(particleCount)),
    i,j,k=0;
    float smoothingRad = particleSystem->getSmoothingRadius();
    glm::vec3 position(0), position2(0, cube->getCenter().y + cube->getHalfDimensions().y*0.5, 0), velocity(0,0,0);
    
    int phase = currPhaseCube;
    float mass = 1.f;
    
    for(i=0; i<damDim; i++)
    {
        for(j=0; j<damDim; j++)
        {
            for(k=0; k<damDim; k++)
            {
                position = (glm::vec3(i,j,k)*smoothingRad - glm::vec3(float(damDim) * smoothingRad/2.0f)) + position2;
                particleSystem->addParticle(Particle(position, velocity, phase, mass));
//                particleSystem->particleSizData.push_back(glm::vec3(3.0f));
            }
        }
    }
    
    numberOfParticles += particleCount;
    particleSystem->setRestPose(phase);
    currPhaseCube+=2;
}

void Scene::addBallToScene()
{
    //reference: http://stackoverflow.com/questions/9084189/draw-a-sphere-using-3d-pixels-voxels
    
    float radius = particleSystem->getSmoothingRadius();
    
    int lats=8, longs=8;
    int particleCount = 0;

    glm::vec3 position1(0), position2(0, cube->getCenter().y + cube->getHalfDimensions().y*0.5, 0), velocity(0,0,0);

    int phase = currPhaseBall;
    float mass = 1.f;

    int i, j;
    for (i = 0; i <= lats; i++)
    {
        double lat0 = PI * (-0.5 + (double)(i - 1) / lats);
        double z0 = sin(lat0);
        double zr0 = cos(lat0);
        
        for (j = 0; j <= longs; j++)
        {
            double lng = 2 * PI * (double)(j - 1) / longs;
            double x = cos(lng);
            double y = sin(lng);
            
            position1 = glm::vec3(x * zr0, y * zr0, z0) + position2;
            
            particleSystem->addParticle(Particle(position1*radius, velocity, phase, mass));
//            particleSystem->particleSizData.push_back(glm::vec3(3.0f));
            particleCount++;
        }
    }
    
    numberOfParticles += particleCount;
    particleSystem->setRestPose(phase);
    currPhaseBall+=2;
}

void Scene::update(){
    
    particleSystem->update();
}