#pragma once

#include "utility.h"

class Interval {

public:
	double min_, max_;

	Interval() : min_(Infinity), max_(-Infinity) {}
	Interval(double min, double max) : min_(min), max_(max) {}

	double size() const 
	{
		return max_ - min_;
	}

	bool contains(double v) const 
	{
		return v >= min_ && v <= max_;
	}

	bool surrounds(double v) const
	{
		return v > min_ && v < max_;
	}
	
	double clamp(double v) const
	{
		if (v < min_) return min_;
		if (v > max_) return max_;
		return v;
	}

	Interval expand(double delta)
	{
		double padding = delta / 2;
		return Interval(min_ - padding, max_ + padding);
	}
	static const Interval Empty, Universe;
};

Interval intersect(const Interval& a, const Interval& b)
{
	return Interval(max(a.min_, b.min_), min(a.max_, b.max_));
}

Interval unite(const Interval& a, const Interval& b)
{
	return Interval(min(a.min_, b.min_), max(a.max_, b.max_));;
}

Interval operator+(const Interval& a, const double& offset)
{
	return Interval(a.min_ + offset, a.max_ + offset);
}

Interval operator+(const double& offset, const Interval& a)
{
	return a + offset;
}

const Interval Interval::Empty;
const Interval Interval::Universe(-Infinity, Infinity);