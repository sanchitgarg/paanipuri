#pragma once
#include "glm/glm.hpp"
#include <vector>
#include "Particle.h"
#include "utilities.h"
#include <Eigen/Sparse>

typedef Eigen::SparseMatrix<float> SparseMatrix;

class Constraint {
protected:
	
public:
	//TODO: What do I need to init the constraint? anything that is universal?
    Constraint();
	virtual ~Constraint();

	virtual void Solve(glm::vec3& position, const SparseMatrix& invMass);
};

class ContactConstraint : public Constraint {
protected:
	int _particleIndex;

public:
	ContactConstraint();
	ContactConstraint(int particleIndex);
	virtual ~ContactConstraint();

	virtual void Solve(glm::vec3& position, const SparseMatrix& invMass);
	int getParticleIndex();
};

//shape matching constraint also needs stiffness, which by consequence gives it deformables (Which we do not need) but we might need it just in general
class ShapeMatchingConstraint : public Constraint {
protected:
    int _particleIndex;

public:
	ShapeMatchingConstraint();
    ShapeMatchingConstraint(int particleIndex);
	virtual ~ShapeMatchingConstraint();

	virtual void Solve(glm::vec3& position, const SparseMatrix& invMass);
    virtual void Solve(std::vector<int>& particleGroup, std::vector<Particle>& particles);
    
    int getParticleIndex();
};

class DensityConstraint : public Constraint {
protected:
	int _particleIndex;

public:
	DensityConstraint();
    DensityConstraint(int particleIndex);
	virtual ~DensityConstraint();

	virtual void Solve(glm::vec3& position, const SparseMatrix& invMass);
    virtual void Solve(std::vector<Particle>& particles);
    
    //function that returns the density of a particle
    float getDensity(int index, std::vector<Particle>& particles);

    glm::vec3 findDeltaPosition(int index, std::vector<Particle>& particles);
    void findLambda(std::vector<Particle>& particles);
    void viscosity(int index, std::vector<Particle>& particles);
    
    float wPoly6Kernel(glm::vec3 distance, float smoothingRadius);
    glm::vec3 gradientWSpikyKernel(glm::vec3 distance, float smoothingRadius);
    
    glm::vec3 gradientConstraintAtParticle(int index, std::vector<Particle>& particles);
    glm::vec3 gradientConstraintForNeighbor(int index, int neighborIndex, std::vector<Particle>& particles);

    
    const float poly6Const = 315.0 / (64 * PI);
    const float spikyConst = 45.0 / (PI);
    const float restDensity = 1000.0; //1000kg/m3
    const float smoothingRadius = 1.5f;
    float s_6 = smoothingRadius*smoothingRadius*smoothingRadius*
    smoothingRadius*smoothingRadius*smoothingRadius;
    float s_9 = (smoothingRadius*smoothingRadius*smoothingRadius)*s_6;
    const float relaxation = 0.1f;
    
    int getParticleIndex();
};