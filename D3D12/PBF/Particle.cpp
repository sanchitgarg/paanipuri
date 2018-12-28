//
//  Particle.cpp
//  paani
//
//  Created by Sanchit Garg on 2/28/15.
//  Copyright (c) 2015 Debanshu. All rights reserved.
//

#include "pch.h"
#include "Particle.h"
#include <iostream>
#include "utilities.h"
//static variables

//float Particle::mass = 1.0f;
//float Particle::radius = 1.0f;

//Constructors
Particle::Particle()
{
    this->position = glm::vec3(0,0,0);
    this->velocity = glm::vec3(0,0,0);
    this->predictedPosition = glm::vec3(0,0,0);
    this->deltaPi = glm::vec3(0,0,0);
    this->phase = 0;
    this->mass = 1;
}

Particle::Particle(glm::vec3 pos)
{
    this->position = pos;
    this->velocity = glm::vec3(0,0,0);
    this->predictedPosition = glm::vec3(0,0,0);
    this->deltaPi = glm::vec3(0,0,0);
    this->phase = 0;
    this->mass = 1;
}

Particle::Particle(glm::vec3 pos, glm::vec3 vel)
{
    this->position = pos;
    this->velocity = vel;
    this->predictedPosition = glm::vec3(0,0,0);
    this->deltaPi = glm::vec3(0,0,0);
    this->phase = 0;
    this->mass = 1;
}

Particle::Particle(glm::vec3 pos, glm::vec3 vel, int p, float m = 1.0f)
{
    this->position = pos;
    this->velocity = vel;
    this->predictedPosition = glm::vec3(0,0,0);
    this->deltaPi = glm::vec3(0,0,0);
    this->phase = p;
    this->mass = m;
}

//Getter functions
float Particle::getMass()
{
    return this->mass;
}

void Particle::setMass(float mass) {
    this->mass = mass;
}

float Particle::getRadius()
{
    return this->radius;
}

glm::vec3 Particle::getPosition()
{
    return this->position;
}

glm::vec3 Particle::getVelocity()
{
    return this->velocity;
}

glm::vec3 Particle::getForces()
{
    return this->forces;
}

glm::vec3 Particle::getPredictedPosition()
{
    return this->predictedPosition;
}

float Particle::getLambda()
{
    return lambda;
}

glm::vec3 Particle::getDeltaPi()
{
    return deltaPi;
}

float Particle::getDensity(){
    if(phase < 2)
        return density;
    else
        return 0.0;
}

void Particle::setDensity(float density)
{
    this->density = density;
}

std::vector<int> Particle::getNeighborIndices()
{
    return neighborIndices;
}

glm::ivec3 Particle::getHashPosition()
{
    return hashPosition;
}

//Setter functions
void Particle::setPosition(glm::vec3 pos)
{
    this->position = pos;
}

void Particle::setVelocity(glm::vec3 vel)
{
    if (glm::length(vel)>100) {
        this->velocity = 0.1f*vel;
    }
    else this->velocity = vel;
}

void Particle::setForces(glm::vec3 netForce)
{
    this->forces = netForce;
}

void Particle::setPredictedPosition(glm::vec3 pos)
{
//    if(glm::any(glm::isnan(pos)) || glm::any(glm::isinf(pos)))
//    {
//        std::cout<<"[ERROR] predictedPos";
//    }
    this->predictedPosition = pos;
}

void Particle::addNeighborIndex(int index)
{
    neighborIndices.push_back(index);
}

void Particle::clearNeighbors()
{
    neighborIndices.clear();
}

void Particle::setLambda(float l)
{
    lambda = l;
}

void Particle::setDeltaPi(glm::vec3 p)
{
    if(glm::any(glm::isnan(p)) || glm::any(glm::isinf(p)))
    {
//        std::cout<<"[ERROR] deltaPi";
        deltaPi = glm::vec3(EPSILON);
    }
    else deltaPi = p;
}

void Particle::setHashPosition(glm::ivec3 p)
{
    hashPosition = p;
}

float Particle::getPredictedMass() {
    return predictedMass;
}

void Particle::setPredictedMass(float predictedMass) {
    this->predictedMass = predictedMass;
}

int Particle::getPhase() {
    return phase;
}

void Particle::setPhase(int phase) {
    this->phase = phase;
}

glm::vec3 Particle::getRestOffset() {
    return this->restOffset;
}

void Particle::setRestOffset(glm::vec3 restOffset) {
    this->restOffset = restOffset;
}