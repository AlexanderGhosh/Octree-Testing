#pragma once
#include <glm.hpp>
#include "Ray.h"

struct BoundingBox {
	union {
		struct {
			glm::vec3 max, min;
		};
		glm::vec3 corners[2];
	};
	bool hit;
	BoundingBox() : max(0), min(0), hit(false) { }
	BoundingBox(glm::vec3 max, glm::vec3 min) : max(max), min(min), hit(false) { }

	bool Contains(glm::vec3 point) {
		return glm::all(glm::lessThanEqual(point, max) && glm::greaterThanEqual(point, min));
	}

	/*
	Fast Ray-Box Intersection
	by Andrew Woo
	from "Graphics Gems", Academic Press, 1990
	*/
	bool Intersects(Ray ray, glm::vec3& hitPoint) {
		const char RIGHT = 0;
		const char LEFT = 1;
		const char MIDDLE = 2;

		bool inside = true;
		glm::i8vec3 quadrant;
		int i;

		int whichPlane;

		glm::vec3 maxT, candidatePlane;

		for (i = 0; i < 3; i++) {
			if (ray.origin[i] < min[i]) {
				quadrant[i] = LEFT;
				candidatePlane[i] = min[i];
				inside = false;
			}
			else if (ray.origin[i] > max[i]) {
				quadrant[i] = RIGHT;
				candidatePlane[i] = max[i];
				inside = false;
			}
			else {
				quadrant[i] = MIDDLE;
			}
		}

		if (inside) {
			hitPoint = ray.origin;
			return true;
		}

		for (i = 0; i < 3; i++) {
			if (quadrant[i] != MIDDLE && ray.dir[i] != 0) {
				maxT[i] = (candidatePlane[i] - ray.origin[i]) / ray.dir[i];
			}
			else {
				maxT[i] = -1;
			}
		}

		whichPlane = 0;
		for (i = 0; i < 3; i++) {
			if (maxT[whichPlane] < maxT[i]) {
				whichPlane = i;
			}
		}

		if (maxT[whichPlane] < 0) {
			return false;
		}

		for (i = 0; i < 3; i++) {
			if (whichPlane != i) {
				hitPoint[i] = ray.origin[i] + maxT[whichPlane] * ray.dir[i];
				if (hitPoint[i] < min[i] || hitPoint[i] > max[i]) {
					return false;
				}
			}
			else {
				hitPoint[i] = candidatePlane[i];
			}
		}
		return true;
	}

	glm::vec3 Length() {
		return max - min;
	}

	glm::vec3 Center() {
		return max - Length() * 0.5f;
	}

	BoundingBox operator + (const glm::vec3& b)  const {
		return BoundingBox(max + b, min + b);
	}

	BoundingBox operator += (const glm::vec3& b) {
		max += b;
		min += b;
		return *this;
	}

	BoundingBox operator - (const glm::vec3& b) const {
		return BoundingBox(max - b, min - b);
	}

	BoundingBox operator / (const float& b) const {
		return BoundingBox(max / b, min / b);
	}

	BoundingBox operator * (const float& b) const {
		return BoundingBox(max * b, min * b);
	}
};