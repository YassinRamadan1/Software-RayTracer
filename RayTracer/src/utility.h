#pragma once
#include <iostream>
#include <fstream>
#include <cmath>
#include <cstdlib>
#include <vector>
#include <algorithm>
#include <limits>
#include <memory>
#include <windows.h>
#include <thread>
#include <execution>
const double Infinity = std::numeric_limits<double>::infinity();
const double Pi = 3.1415926535897932385;

inline double RadiansToDegrees(double angle)
{
	return 180 / Pi * angle;
}

inline double DegreesToRadians(double angle)
{
	return Pi / 180 * angle;
}

inline double RandomDouble(double min = 0, double max = 1)
{
	return min + (max - min) * std::rand() / (RAND_MAX + 1.0f);
}

inline int RandomInteger(int min = 0, int max = 1)
{
	return int(RandomDouble(min, max + 1));
}

template<typename T>
inline T Lerp(T st, T en, double t)
{
	return (1 - t) * st + t * en;
}

inline double Smoothstep(double min, double max, double t)
{
	if (t < min)
		return 0;
	else if (t > max)
		return 1;
	return t * t * (3 - 2 * t);
}

inline double Smootherstep(double t)
{
	return t * t * t * (t * (t * 6.0 - 15.0) + 10.0);
}