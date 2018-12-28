#pragma once
#include <glm/glm.hpp>

class Collision {
public:
	int _id; //id of colliding particle
	glm::vec3 _normal;
	double _distance;

	Collision(const int id, const glm::vec3 normal, const double distance);
	~Collision();
};

