//
//  scene.h
//  paani
//
//  Created by Sanchit Garg on 3/12/15.
//  Copyright (c) 2015 Debanshu. All rights reserved.
//

#ifndef __paani__scene__
#define __paani__scene__

//#define GLEW_STATIC
//#include <GL/glew.h>
//#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// #include "imageloader.h"
#include "ParticleSystem.h"

// constants
const glm::vec2 SCREEN_SIZE(800, 600);

class Cube;
class Scene;

extern Scene* scene;

class Cube
{
private:
    glm::vec3 dimensions;
    glm::vec3 center;
    float cellSize;     //the size of each hash grid cell
    
public:
    
    //constructors
    Cube(){};
    Cube(glm::vec3, glm::vec3); //Input center and dimensions
    
    // Getters
    glm::vec3 getDimensions();
    glm::vec3 getHalfDimensions();
    glm::vec3 getCenter();
    float getCellSize();
    
    //Setters
    void setDimension(glm::vec3);
    void setCenter(glm::vec3);
    void setCellSize(float);
};

class Scene
{
public:
    Scene();
    void init();
    void update();
    
    glm::vec3 gravity;
    int numberOfParticles;
    Cube* cube;
    ParticleSystem* particleSystem;
    
    Mesh mesh;
    
    void addParticlesToScene(int);
    void addBallToScene();
    void addCubeToScene();
    void pourFluid(int);
};

#endif /* defined(__paani__scene__) */