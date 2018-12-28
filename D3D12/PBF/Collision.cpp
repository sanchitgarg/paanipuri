#include "pch.h"
#include "Collision.h"


Collision::Collision(const int id, const glm::vec3 normal, const double distance) {
	_id = id;
	_normal = normal;
	_distance = distance;
}


Collision::~Collision() {

}
