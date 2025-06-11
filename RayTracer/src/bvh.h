#pragma once

#include "hittable_list.h"

class BVHNode : public Hittable
{

	AABB bounding_box_;
	std::shared_ptr<Hittable> left, right;

	static bool boundingBoxXComparator(std::shared_ptr<Hittable> a, std::shared_ptr<Hittable> b)
	{
		return a->getBoundingBox().x_.min_ < b->getBoundingBox().x_.min_;
	}

	static bool boundingBoxYComparator(std::shared_ptr<Hittable> a, std::shared_ptr<Hittable> b)
	{
		return a->getBoundingBox().y_.min_ < b->getBoundingBox().y_.min_;
	}

	static bool boundingBoxZComparator(std::shared_ptr<Hittable> a, std::shared_ptr<Hittable> b)
	{
		return a->getBoundingBox().z_.min_ < b->getBoundingBox().z_.min_;
	}

public:

	BVHNode(HittableList list) : BVHNode(list.objects_, 0, list.objects_.size()) {}

	BVHNode(std::vector<std::shared_ptr<Hittable>>& objects, size_t start, size_t end)
	{
		bounding_box_ = AABB::Empty;
		for (size_t i = start; i < end; i++)
		{
			bounding_box_ = AABB(bounding_box_, objects[i]->getBoundingBox());
		}
		size_t number_of_objects = end - start;

		if (number_of_objects == 1)
		{
			left = right = objects[start];
		}
		else if (number_of_objects == 2)
		{
			left = objects[start];
			right = objects[start + 1];
		}
		else
		{
			int axis = bounding_box_.longestAxis();
			auto comparator = ((axis == 0) ? boundingBoxXComparator : ((axis == 1) ? boundingBoxYComparator : boundingBoxZComparator));

			std::sort(std::begin(objects) + start, std::begin(objects) + end, comparator);
			size_t mid = start + number_of_objects / 2;

			left = std::make_shared<BVHNode>(objects, start, mid),
				right = std::make_shared<BVHNode>(objects, mid, end);
		}
	}

	bool hit(const Ray& r, Interval ray_t, HitRecord& rec) const override
	{
		if (!bounding_box_.hit(r, ray_t))
			return false;
		bool hit_left = left->hit(r, ray_t, rec);
		bool hit_right = right->hit(r, Interval(ray_t.min_, hit_left ? rec.t_ : ray_t.max_), rec);

		return hit_right || hit_left;
	}

	AABB getBoundingBox() const override
	{
		return bounding_box_;
	}
};