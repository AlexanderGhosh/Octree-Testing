#pragma once
#include <glm.hpp>

struct Ray {
	glm::vec3 origin, dir;
	float maxDir;
	bool limitedDir;
	Ray() : origin(0), dir(0), maxDir(0), limitedDir(0) { }
	Ray(glm::vec3 ori, glm::vec3 dir) : Ray() {
		origin = ori;
		this->dir = dir;
	}
	Ray(glm::vec3 ori, glm::vec3 dir, float dist) : Ray() {
		origin = ori;
		this->dir = dir;
		limitedDir = true;
		maxDir = dist;
	}
};
