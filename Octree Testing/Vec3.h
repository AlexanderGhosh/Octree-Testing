#pragma once
#include <string>

struct Vec3 {
	union {
		struct { 
			float x, y, z; 
		};
		float a[3];
	};
	Vec3() : x(0), y(0), z(0) {}
	Vec3(float a) : x(a), y(a), z(a) {}
	Vec3(float x, float y, float z) : x(x), y(y), z(z) {}

	std::string ToString() {
		return "(" + std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z) + ")";
	}

	bool operator > (const Vec3& b) const {
		const Vec3& a = *this;
		return a.x > b.x && a.y > b.y && a.z > b.z;
	}

	bool operator < (const Vec3& b) const {
		const Vec3& a = *this;
		return a.x < b.x && a.y < b.y && a.z < b.z;
	}

	Vec3 operator + (const Vec3& b) const {
		const Vec3& a = *this;
		return Vec3(
			a.x + b.x,
			a.y + b.y,
			a.z + b.z);
	}

	Vec3 operator += (const Vec3& b) {
		Vec3& a = *this;
		a.x += b.x;
		a.y += b.y;
		a.z += b.z;
		return a;
	}

	Vec3 operator - (const Vec3& b) const {
		const Vec3& a = *this;
		return Vec3(
			a.x - b.x, 
			a.y - b.y, 
			a.z - b.z);
	}

	Vec3 operator -(const float& b) const {
		const Vec3& a = *this;
		return Vec3(
			a.x - b,
			a.y - b,
			a.z - b);
	}

	Vec3 operator /(const float& b) const {
		const Vec3& a = *this;
		return Vec3(
			a.x / b,
			a.y / b,
			a.z / b);
	}

	Vec3 operator *(const float& b)  const {
		const Vec3& a = *this;
		return Vec3(
			a.x * b,
			a.y * b,
			a.z * b);
	}
};