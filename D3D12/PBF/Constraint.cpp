#include "pch.h"
#include "Constraint.h"
#include "scene.h"
#include <iostream>
#include <tbb/tbb.h>
#include <tbb/parallel_for.h>
#include <tbb/blocked_range.h>
#include <Eigen/SVD>
#include <unsupported/Eigen/MatrixFunctions>

using namespace tbb;

bool checkPhase = true;

Constraint::Constraint() {
}


Constraint::~Constraint() {

}

void Constraint::Solve(glm::vec3& position, const SparseMatrix& invMass) {
    //so check if there is a contact on the particle. if so, resolve it
    
}

//Contact Constraint -------------------------------------------------------------
ContactConstraint::ContactConstraint() {

}

ContactConstraint::ContactConstraint(int particleIndex) {
	_particleIndex = particleIndex;
}

ContactConstraint::~ContactConstraint() {
    
}

//TODO: Implement
void ContactConstraint::Solve(glm::vec3& position, const SparseMatrix& invMass) {
	//so check if there is a contact on the particle. if so, resolve it

}
//---------------------------------------------------------------------------------


//Shape Matching Constraint -------------------------------------------------------
ShapeMatchingConstraint::ShapeMatchingConstraint() {
    
}

ShapeMatchingConstraint::ShapeMatchingConstraint(int particleIndex) {
    _particleIndex = particleIndex;
}

ShapeMatchingConstraint::~ShapeMatchingConstraint() {
    
}

void ShapeMatchingConstraint::Solve(glm::vec3& position, const SparseMatrix& invMass){
    
}

//void ShapeMatchingConstraint::Solve(std::vector<Particle> particleGroup){
//    
//    //first we have to find the center of mass of the particles in the deformed configuration.
//    //that should just be the average position of all the particles?
//    glm::vec3 positionSum = glm::vec3(0.0f);
//    for(int i = 0; i < particleGroup.size(); i++) {
//        positionSum += particleGroup[i].getPredictedPosition(); //should be predicted position right? not position...
//    }
//    glm::vec3 c = positionSum / (float) particleGroup.size();
//    
//    //Imagine we have the matrix A already
//    Matrix A;
//    //Eigen::JacobiSVD<Eigen::MatrixXf> svd(A, Eigen::ComputeFullU | Eigen::ComputeFullV);
//    Eigen::JacobiSVD<Matrix> svd(A, Eigen::ComputeFullU | Eigen::ComputeFullV);
//    //solve for Q (rotation matrix
//    Matrix Q = svd.matrixU() * svd.matrixV().transpose();
//}

    //now solve delta x for particle i. so this will be a loop over all particles in the group?
void ShapeMatchingConstraint::Solve(std::vector<int>& particleGroup, std::vector<Particle>& particles)
{
    if(!particleGroup.size())
    {
        return;
    }
    
    glm::vec3 centerMassDeformed(0.0), centerMassRest;
    glm::vec3 x1, y1;
    
    float mass = particles.at(particleGroup.at(0)).getMass();
    
    //Calculating Center of Mass (as mass of all partiles is the same, it becomes the average of positions
    for(int i=0; i<particleGroup.size(); i++)
    {
        centerMassDeformed += particles.at(particleGroup.at(i)).predictedPosition;
    }
    centerMassDeformed /= particleGroup.size();
    
    glm::vec3 q(0), p(0), rParticle(0);
    glm::mat3 Apq(0);
    Eigen::Matrix3f A_pq; //for using eigen. A_pq should be set to Apq

    //Finding Apq matrix
    for(int i=0;i<particleGroup.size(); i++)
    {
        p = particles.at(particleGroup.at(i)).predictedPosition - centerMassDeformed;
        q = particles.at(particleGroup.at(i)).getRestOffset();

        Apq += mass * p * q;
    }
    
    //Change from mat3 to eigen
    for(int i=0; i<3; i++)
    {
        for(int j=0; j<3; j++)
        {
            A_pq(i,j) = Apq[i][j];
        }
    }
    
//    Eigen::JacobiSVD<Eigen::Matrix3f> svd(Aeigen, Eigen::ComputeFullU | Eigen::ComputeFullV);
//    Eigen::Matrix3f Q = svd.matrixU() * svd.matrixV().transpose();
//    std::cout<<svd.singularValues();
    
    glm::mat3 rotMat(1.0);
    bool isSqrtable = true;
    
    //Find S
    Eigen::Matrix3f S,Q;
    S = (A_pq.transpose() * A_pq);
    
    Eigen::Vector3cf S_eigenvalues = S.eigenvalues();
    for (int i=0; i<S_eigenvalues.size(); i++){
//        std::cout<<S_eigenvalues[i].real();
        if (S_eigenvalues[i].real()<=0 && S_eigenvalues[i].imag() == 0){
            rotMat = glm::mat3(1.0);
            isSqrtable = false;
            break;
        }
    }
    
    if (isSqrtable) {
        S = S.sqrt();
        Q = A_pq*S.inverse();
        
        float t = Q.determinant();
        
        //If the determinant of the rotation matrix is not ~(1 or -1), discard it
        if( ! ((t < 1.f+ZERO_ABSORPTION_EPSILON && t > 1.f-ZERO_ABSORPTION_EPSILON) || (t < -1.f+ZERO_ABSORPTION_EPSILON && t > -1.f-ZERO_ABSORPTION_EPSILON)) )
        {
//            std::cout<<"ERROR"<<std::endl;
//            std::cout<<t;
        }
        
        //else convert rotation matrix from eigen to mat3
        else
        {
            for(int i=0; i<3; i++)
            {
                for(int j=0; j<3; j++)
                {
                    rotMat[i][j] = Q(i,j);
                }
            }
        }
    }

//    if(Q.determinant() < 0.0f)
//    {
//        Eigen::Matrix3f correction = Eigen::Matrix3f::Identity();
//        //correction(2,2) = -1.f;
//
//        Q = (svd.matrixU() * correction) * svd.matrixV().transpose();
//    }
    
    
    glm::vec3 deltaPi = (rotMat*particles.at(_particleIndex).getRestOffset() + centerMassDeformed) - particles.at(_particleIndex).predictedPosition;
    particles.at(_particleIndex).setDeltaPi(deltaPi);
}

int ShapeMatchingConstraint::getParticleIndex()
{
    return _particleIndex;
}

//---------------------------------------------------------------------------------


//Density Constraint --------------------------------------------------------------

DensityConstraint::DensityConstraint() {

}

DensityConstraint::DensityConstraint(int particleIndex) {
	_particleIndex = particleIndex;
}

DensityConstraint::~DensityConstraint() {
	
}

void DensityConstraint::Solve(glm::vec3& position, const SparseMatrix& invMass) {
	
}

//TODO: setup density constraint to find lambda
void DensityConstraint::Solve(std::vector<Particle>& particles) {
    
    glm::vec3 deltaPi = findDeltaPosition(_particleIndex, particles); //constraintDelta*lambda
    Particle& currParticle = particles.at(_particleIndex);
    currParticle.setDeltaPi(deltaPi);
}

void DensityConstraint::findLambda(std::vector<Particle>& particles){

    int index = _particleIndex;
    Particle& currParticle = particles.at(index);
    
    std::vector<int> neighbors = currParticle.getNeighborIndices();
    
    int numNeighbors = static_cast<int>(neighbors.size());
    
    float sumGradientAtParticle = 0.0f;
    float gradientNeighbor = 0.0f;
    
    for (int i=0; i<numNeighbors; i++) {
        
        if(particles.at(neighbors.at(i)).getPhase() < 2)
        {
            gradientNeighbor = glm::length(gradientConstraintForNeighbor(index, i, particles));
            sumGradientAtParticle += gradientNeighbor * gradientNeighbor / currParticle.getMass();
        }
    }
    
    float gradientParticle = glm::length(gradientConstraintAtParticle(index, particles));
    
    //Different Masses
    sumGradientAtParticle += (gradientParticle * gradientParticle);// / currParticle.getMass();
    
    float currDensity = this->getDensity(index, particles);
    float densityContraint = (currDensity/restDensity) - 1.0f; //density constraint
    
    float lambdaI = -1.0f * (densityContraint/(sumGradientAtParticle+relaxation)); //findLambda using density constraint
    
//    if(std::isnan(lambdaI)||std::isinf(fabs(lambdaI)))
//    {
//        std::cout<<"[ERROR] findLambda";
//    }
    // do these have to be atomic?
    currParticle.setDensity(currDensity);
    currParticle.setLambda(lambdaI);
}

float DensityConstraint::getDensity(int index, std::vector<Particle>& particles)
{
    float density = 0;
    int i;
    Particle& currParticle = particles.at(index);
    
    std::vector<int> neighbors = currParticle.getNeighborIndices();
    
    //Kernel function implementation
    //  Using poly6 kernel function
    
    //TODO
    //  Currently mass is 1 for all particles
    //  If mass is changed, change the for loop to multiply by mass
    for(i=0; i<neighbors.size(); i++)
    {
        //TODO
        if(particles.at(neighbors.at(i)).getPhase() < 2)
        {
            glm::vec3 temp = (currParticle.getPredictedPosition() - particles.at(neighbors.at(i)).getPredictedPosition());
            
    //        if(glm::any(glm::isnan(currParticle.getPredictedPosition())))
    //        {
    //            std::cout<<"[ERROR] getDensity at Particles";
    //        }
    //        if(glm::any(glm::isnan(particles.at(neighbors.at(i)).getPredictedPosition())) || glm::any(glm::isinf(particles.at(neighbors.at(i)).getPredictedPosition())))
    //        {
    //            utilityCore::printVec3(particles.at(neighbors.at(i)).getPredictedPosition());
    //            std::cout<<"[ERROR] getDensity at Neighbours";
    //        }
            
            density +=  particles.at(neighbors.at(i)).getMass() * wPoly6Kernel(temp, smoothingRadius);
        }
    }
    
    if(density < EPSILON)
    {
        density = EPSILON;
    }
    else if(density > restDensity-1)
    {
        density = restDensity - 1;
    }
    
    return density;
}

float DensityConstraint::wPoly6Kernel(glm::vec3 distance, float smoothingRadius)
{
//    if(glm::any(glm::isnan(distance)) || glm::any(glm::isinf(distance)))
//    {
//        std::cout<<"[ERROR] wPoly6Kernel";
//    }
     
    
    float d = glm::length(distance);
    float x = (smoothingRadius*smoothingRadius) - d*d;
    float x_3 = x*x*x;
    
    float t = poly6Const * x_3/s_9;
    
    if(isinf(fabs(t)) || isnan(t))
    {
        t = EPSILON;
    }
    return t;
}

glm::vec3 DensityConstraint::gradientWSpikyKernel(glm::vec3 distance, float smoothingRadius)
{
    float distanceLength = glm::length(distance);
    
    float x = (smoothingRadius-distanceLength)*(smoothingRadius-distanceLength);
    
    float gradientW = spikyConst * (1.0f/s_6) * x * 1.0f/(distanceLength+EPSILON);
    
    glm::vec3 retVal = gradientW*distance;
    
//    if(glm::any(glm::isinf(retVal)))
//    {
//        retVal = glm::vec3(EPSILON);
//    }
    
    return retVal;
}

glm::vec3 DensityConstraint::gradientConstraintAtParticle(int index, std::vector<Particle>& particles)
{
    glm::vec3 gradientReturn(0,0,0);
    float restDensityInverse = 1.0/restDensity;
    
    std::vector<int> neighbors = particles.at(index).getNeighborIndices();
    
    for(int i=0; i<neighbors.size(); i++)
    {
        if(particles.at(neighbors.at(i)).getPhase() < 2)
        {
            gradientReturn += restDensityInverse *
            gradientWSpikyKernel((particles.at(index).getPosition() - particles.at(neighbors.at(i)).getPosition()), smoothingRadius);
        }
    }
    
    return gradientReturn;
}

glm::vec3 DensityConstraint::gradientConstraintForNeighbor(int index, int neighborIndex, std::vector<Particle>& particles)
{
    glm::vec3 gradientReturn(0,0,0);
    float restDensityInverse = 1.0/restDensity;
    
    
    gradientReturn = restDensityInverse * gradientWSpikyKernel(particles.at(index).getPosition() - particles[neighborIndex].getPosition(), smoothingRadius);
    
    return (-1.0f * gradientReturn);
}

glm::vec3 DensityConstraint::findDeltaPosition(int index, std::vector<Particle>& particles)
{
    glm::vec3 deltaPi(0,0,0);
    
    Particle& currParticle = particles.at(index);
    
    std::vector<int> neighbors = currParticle.getNeighborIndices();
    float lambda_i = currParticle.getLambda();
    float sCorr = 0, k = 0.1, deltaQ = 0.1 * smoothingRadius;
    //    int n = 4; //avoid using pow
    float artificialTerm;
    
    // constraintDelta * lambda
    for(int i=0; i<neighbors.size(); i++)
    {
//        if (glm::any(glm::isinf(particles.at(neighbors.at(i)).getPredictedPosition()))){
//            std::cout<<neighbors.at(i)<<std::endl;
//        }

        if(particles.at(neighbors.at(i)).getPhase() < 2)
        {
        
            float temp = wPoly6Kernel(glm::vec3(deltaQ, 0, 0), smoothingRadius);
            
            artificialTerm = wPoly6Kernel((currParticle.getPredictedPosition() - particles.at(neighbors.at(i)).getPredictedPosition()), smoothingRadius) / (temp+EPSILON);

    //        if (glm::any(glm::isinf(particles.at(neighbors.at(i)).getPredictedPosition()))){
    //            std::cout<<neighbors.at(i)<<std::endl;
    //        }
            
            sCorr = -1.0 * k * artificialTerm * artificialTerm * artificialTerm * artificialTerm;
            
            deltaPi += (particles.at(neighbors[i]).getLambda() + lambda_i + sCorr) *
            gradientWSpikyKernel((currParticle.getPredictedPosition() - particles.at(neighbors.at(i)).getPredictedPosition()), smoothingRadius);
        }
    }
    
    return (deltaPi/restDensity);
}

void DensityConstraint::viscosity(int index, std::vector<Particle>& particles)
{
    Particle& currParticle = particles.at(index);
    std::vector<int> neighbors = currParticle.getNeighborIndices();
    
    glm::vec3 newVelocity = currParticle.getVelocity();
    
    for(int i=0; i<neighbors.size(); i++)
    {
        newVelocity += (1/particles.at(neighbors[i]).getDensity()) * (particles.at(neighbors[i]).getVelocity() - currParticle.getVelocity()) * wPoly6Kernel( (currParticle.getPredictedPosition() - particles.at(neighbors.at(i)).getPredictedPosition()), smoothingRadius);
    }
    
    currParticle.setVelocity(newVelocity);
}

int DensityConstraint::getParticleIndex()
{
    return _particleIndex;
}