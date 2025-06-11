#pragma once
#include "Vec.h"

class Ray {

public:
	double time_;
	vec3 orig_, dir_;

	Ray() {}

	Ray(vec3 origin, vec3 direction) :orig_(origin), dir_(direction) {}

	Ray(vec3 origin, vec3 direction, double time) : orig_(origin), dir_(direction), time_(time) {}

	vec3 At(double t) const{

		vec3 temp = t * dir_;
		return orig_ + temp;
	}
};