#pragma once
#include <iostream>
#include <cmath>
#include "tgaimage.h"
#include "interval.h"

class Vec3 {

public:

	union {

		struct {
			double x, y, z;
		};

		struct {
			double r, g, b;
		};

		double data[3];
	};

	Vec3() : x(0), y(0), z(0) {}
	Vec3(double x, double y, double z) : x(x), y(y), z(z) {}
	Vec3(double value) : Vec3(value, value, value) {}

	double operator [](int i) {

		if (i >= 3) {
			throw std::runtime_error("Access out of bound");
		}
		return data[i];
	}

	const double& operator [](size_t i) const {

		if (i >= 3) {
			throw std::runtime_error("Access out of bound");
		}
		return data[i];
	}

	Vec3 operator -() const {

		return Vec3(-x, -y, -z);
	}

	Vec3& operator +=(const Vec3& other) {

		x += other.x;
		y += other.y;
		z += other.z;
		return *this;
	}

	Vec3& operator -=(const Vec3& other) {

		x -= other.x;
		y -= other.y;
		z -= other.z;
		return *this;
	}

	Vec3& operator *=(const double v) {

		x *= v;
		y *= v;
		z *= v;
		return *this;
	}

	Vec3& operator /=(const double v) {

		if (v == 0) {
			throw std::runtime_error("Division by zero");
		}

		x /= v;
		y /= v;
		z /= v;
		return *this;
	}

	double length() const {
		return sqrt(length2());
	}

	double length2() const {
		return x * x + y * y + z * z;
	}

	static Vec3 random(double min = 0, double max = 1) {
	
		return Vec3(RandomDouble(min, max), RandomDouble(min, max), RandomDouble(min, max));
	}

	static Vec3 randomUnitVector();

	static Vec3 randomOnHemisphere(const Vec3& normal);

	static Vec3 randomInUnitDisk();

	bool nearZero() const {

		const double s = 1e-8;
		return (fabs(x) < s) && (fabs(y) < s) && (fabs(z) < s);
	}
};

inline std::ostream& operator<<(std::ostream& out, const Vec3& vec) {

	out << vec.x << ' ' << vec.y << ' ' << vec.z;
	return out;
}

inline Vec3 operator +(Vec3 vec1, Vec3 vec2) {

	return Vec3(vec1.x + vec2.x, vec1.y + vec2.y, vec1.z + vec2.z);
}

inline Vec3 operator -(Vec3 vec1, Vec3 vec2) {

	return Vec3(vec1.x - vec2.x, vec1.y - vec2.y, vec1.z - vec2.z);
}

inline Vec3 operator *(Vec3 vec1, Vec3 vec2) {

	return Vec3(vec1.x * vec2.x, vec1.y * vec2.y, vec1.z * vec2.z);
}

inline Vec3 operator *(Vec3 vec, double v) {

	return Vec3(vec.x * v, vec.y * v, vec.z * v);
}

inline Vec3 operator *(double v, Vec3 vec) {

	return vec * v;
}

inline Vec3 operator /(Vec3 vec, double v) {

	if (v == 0) {
		throw std::runtime_error("Division by zero");
	}

	return vec * (1 / v);
}

inline double dot(Vec3 vec1, Vec3 vec2) {

	return vec1.x * vec2.x + vec1.y * vec2.y + vec1.z * vec2.z;
}

inline Vec3 cross(Vec3 vec1, Vec3 vec2) {

	return Vec3(vec1.y * vec2.z - vec2.y * vec1.z, vec1.z * vec2.x - vec2.z * vec1.x, vec1.x * vec2.y - vec2.x * vec1.y);
}

inline Vec3 normalize(Vec3 vec1) {

	double len = vec1.length();
	if (len == 0) {
		throw std::runtime_error("Zero length vector");
	}
	return Vec3(vec1.x / len, vec1.y / len, vec1.z / len);
}

inline Vec3 reflect(const Vec3& v, const Vec3& n)
{
	return v - 2 * dot(v, n) * n;
}

inline Vec3 refract(const Vec3& v, const Vec3& n, double ratio) 
{
	double length = v.length();
	Vec3 incident = v / length;
	double c1 = abs(dot(incident, n));
	double c2 = sqrt(1 - ratio * ratio * (1 - c1 * c1));

	return length * (ratio * incident + (ratio * c1 - c2) * n);
}

Vec3 Vec3::randomUnitVector() {

	while (true) {
		Vec3 p = Vec3::random(-1, 1);
		double len = p.length2();
		if (len > 1e-100 && len <= 1)
			return p /= sqrt(len);
	}
}

Vec3 Vec3::randomOnHemisphere(const Vec3& normal) {

	Vec3 p = Vec3::randomUnitVector();
	if (dot(p, normal) < 0)
		p = -p;
	return p;
}

Vec3 Vec3::randomInUnitDisk() {

	while (true) {
		Vec3 p = Vec3(RandomDouble(-1, 1), RandomDouble(-1, 1), 0);
		if (p.length2() <= 1)
			return p;
	}
}

inline Vec3 toColor(TGAColor tgaColor)
{
	return Vec3(tgaColor[2] / 255.0, tgaColor[1] / 255.0, tgaColor[0] / 255.0);
}

inline TGAColor toTGAColor(Vec3 color) 
{
	TGAColor tga;
	Interval intensity(0, 0.999);

	tga[0] = static_cast<unsigned char>(intensity.clamp(color.b) * 256);
	tga[1] = static_cast<unsigned char>(intensity.clamp(color.g) * 256);
	tga[2] = static_cast<unsigned char>(intensity.clamp(color.r) * 256);

	//tga[3] = static_cast<unsigned char>(color.a * 255.9999f);
	return tga;
}

inline Vec3 linearToGamma(Vec3 color)
{
	color.x = pow(color.x, 1 / 2.2);
	color.y = pow(color.y, 1 / 2.2);
	color.z = pow(color.z, 1 / 2.2);
	return color;
}

inline Vec3 gammaToLinear(Vec3 color)
{
	color.x = pow(color.x, 2.2);
	color.y = pow(color.y, 2.2);
	color.z = pow(color.z, 2.2);
	return color;
}

using vec3 = Vec3;
using Color = Vec3;