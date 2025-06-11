#pragma once

#include"Ray.h"

class AABB
{

    void padIntervals()
    {
        double delta = 0.0001;
        if (x_.size() < delta) 
            x_ = x_.expand(delta);
        if (y_.size() < delta)
            y_ = y_.expand(delta);
        if (z_.size() < delta)
            z_ = z_.expand(delta);
    }
public:
    
    union
    {
        struct
        {
            Interval x_, y_, z_;
        };

        Interval data_[3];
    };

    AABB() {} // The default AABB is empty, since intervals are empty by default.

    AABB(const Interval& x, const Interval& y, const Interval& z) : x_(x), y_(y), z_(z)
    {
        padIntervals();
    }

    AABB(const vec3& a, const vec3& b)
    {   // Treat the two points a and b as extrema for the bounding box, so we don't require a
        x_ = (a[0] <= b[0]) ? Interval(a[0], b[0]) : Interval(b[0], a[0]);
        y_ = (a[1] <= b[1]) ? Interval(a[1], b[1]) : Interval(b[1], a[1]);
        z_ = (a[2] <= b[2]) ? Interval(a[2], b[2]) : Interval(b[2], a[2]);
        padIntervals();
    }

    AABB(AABB b1, AABB b2)
    {
        x_ = unite(b1.x_, b2.x_);
        y_ = unite(b1.y_, b2.y_);
        z_ = unite(b1.z_, b2.z_);
        padIntervals();
    }

    int longestAxis() const
    {
        int axis = 0;
        double size = x_.size();
        if (size < y_.size())
           size = y_.size(), axis = 1;
        if (size < z_.size())
            axis = 2;
        return axis;
    }

    bool hit(const Ray& r, Interval ray_t) const
    {
        Interval temp;
        for (int axis = 0; axis < 3; axis++)
        {
            
            const double& ray_center = r.orig_.data[axis];
            const double& ray_direction = r.dir_.data[axis];

            temp.min_ = min((data_[axis].max_ - ray_center) / ray_direction, (data_[axis].min_ - ray_center) / ray_direction);
            temp.max_ = max((data_[axis].max_ - ray_center) / ray_direction, (data_[axis].min_ - ray_center) / ray_direction);
            ray_t = intersect(temp, ray_t);

            if (ray_t.max_ <= ray_t.min_)
                return false;
        }

        return true;
    }

    static const AABB Empty, Universe;
};

const AABB AABB::Empty(Interval::Empty, Interval::Empty, Interval::Empty);
const AABB AABB::Universe(Interval::Universe, Interval::Universe, Interval::Universe);

AABB operator+(const AABB& bounding_box, const vec3& offset)
{
    return AABB(bounding_box.x_ + offset.x, bounding_box.y_ + offset.y, bounding_box.z_ + offset.z);
}

AABB operator+(const vec3& offset, const AABB& bounding_box)
{
    return bounding_box + offset;
}