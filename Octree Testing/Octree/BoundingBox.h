#pragma once
#include <glm.hpp>

struct BoundingBox {
	union {
		struct {
			glm::vec3 max, min;
		};
		glm::vec3 corners[2];
	};
	BoundingBox() : max(0), min(0) { }
	BoundingBox(glm::vec3 max, glm::vec3 min) : max(max), min(min) { }

	bool Contains(glm::vec3 point) {
		return glm::all(glm::lessThan(point, max) && glm::greaterThan(point, min));
	}

	glm::vec3 Length() {
		return max - min;
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