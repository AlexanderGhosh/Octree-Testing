#pragma once
#include "Vec3.h"

struct BoundingBox {
	union {
		struct {
			Vec3 max, min;
		};
		Vec3 corners[2];
	};
	BoundingBox() : max(0), min(0) { }
	BoundingBox(Vec3 max, Vec3 min) : max(max), min(min) { }

	bool Contains(Vec3 point) {
		return point < max && point > min;
	}

	Vec3 Length() {
		return max - min;
	}


	BoundingBox operator + (const Vec3& b)  const {
		return BoundingBox(max + b, min + b);
	}

	BoundingBox operator += (const Vec3& b) {
		max += b;
		min += b;
		return *this;
	}

	BoundingBox operator - (const Vec3& b) const {
		return BoundingBox(max - b, min - b);
	}

	BoundingBox operator / (const float& b) const {
		return BoundingBox(max / b, min / b);
	}

	BoundingBox operator * (const float& b) const {
		return BoundingBox(max * b, min * b);
	}
};