//
//  ParticleSystem.h
//  paani
//
//  Created by Sanchit Garg on 2/28/15.
//  Copyright (c) 2015 Debanshu. All rights reserved.
//

#ifndef __paani__ParticleSystem__
#define __paani__ParticleSystem__

#include "Particle.h"
#include <utility>
#include "utilities.h"
#include <map>
#include "Mesh.h"
#include "Constraint.h"
#include <Eigen/Core>
#include <Eigen/Dense>


typedef Eigen::Matrix<float, 3, 1, 0, 3 ,1> EigenVector3;
typedef Eigen::Matrix<float, Eigen::Dynamic, 1> VectorX;
typedef Eigen::Triplet<float, int> SparseMatrixTriplet;

//This class stores the list of particles and handles their interactions

struct Container
{
    int numIndices;
    std::vector<glm::vec3> triangles;
    std::vector<glm::vec3> normals;
    glm::vec3 boundingCenter;
    float boundingRadius;
};

class ParticleSystem
{
    
private:
    std::vector<Particle> particles;   //List of all particles in the system
    std::vector<DensityConstraint*> densityConstraints;   //List of all density constraints in the system
    std::vector<ShapeMatchingConstraint*> shapeConstraints;   //List of all shape matching constraints in the system
    
    std::map<int, std::vector<int>> particleGroup;  // Maps phase to a vector of particle indices
    
    const float smoothingRadius = 1.5f;
    const int fluidSolverIterations = 10;
    const int solidSolverIterations = 6;
    const int stabilityIterations = 1;
    
    const float timeStep = 0.016f;
    
    glm::vec3 forces;
    glm::vec3 lowerBounds;
    glm::vec3 upperBounds;
    
    float cellSize;
    std::map<int, std::vector<int>> hashGrid; //TODO: look in folly AtomicHashMap
    glm::ivec3 gridDim;
    
    // struct Container container;

    //This will save the list of triangle indices that are in the particular grid.
    // first int maps from the 3D grid location to 1D data.
    std::map<int, std::vector<int> > containerGrid;
    
    //this saves whether the voxel has any triangle or not
    std::vector<bool> containerBool;
    
    SparseMatrix invMassMatrix;
    
    glm::vec3 Eigen2GLM(const EigenVector3& eigen_vector);
    EigenVector3 GLM2Eigen(const glm::vec3& glm_vector);
    
public:
    
    struct Container container;

    typedef std::pair<int, glm::vec3> Neighbor;
    std::vector<glm::vec3> particlePosData;
    std::vector<glm::vec3> particleColData;
    std::vector<glm::vec3> particleSizData;
    
    //Getter functions
    std::vector<Particle>& getAllParticles();   //Returns the list of all particles in the system
    float getSmoothingRadius();
    
    void update();
    glm::vec3 getForces();
    glm::vec3 getLowerBounds();
    glm::vec3 getUpperBounds();
    float getCellSize();
    
    void loadContainer(Mesh& mesh);
    void createContainerGrid();
    
    //setter
    void setForces(glm::vec3 f);
    void setLowerBounds(glm::vec3);
    void setUpperBounds(glm::vec3);
    void setCellSize(float size);
    
    //Other functions
    void addParticle(Particle);               //Add a particle to the system
    
    //Function to return a list of all the neighbors within the specified distance
    // Stored as a pair of index and vector to the neighboring particle
    void findNeighbors(int index);
    void setupConstraints();
    void clearConstraints();
    
    //apply external forces (gravity)
    void applyForces(const int index);
    
    //collision detection and resolution
    void particleCollision(int index);
    void particleParticleCollision(int index);
    void particleBoxCollision(int index);
    void particleContainerCollision(int index);
    
    void initialiseHashPositions(int index);         //function that initialises particles hash positions
    
    bool isValidCell(glm::ivec3);
    
    void setupInvMassMatrix();
    
    void setRestPose(int groupID);
};

#endif
