#pragma once

#include "utility.h"
#include "hittable.h"

class HittableList : public Hittable {

private:
	
	AABB bounding_box_;
public:

	std::vector<std::shared_ptr<Hittable>> objects_;

	HittableList() {}

	HittableList(std::shared_ptr<Hittable> object) {
	
		this->add(object);
	}

	void add(std::shared_ptr<Hittable> object) {

		objects_.push_back(object);
		bounding_box_ = AABB(bounding_box_, object->getBoundingBox());
	}

	void clear() {

		objects_.clear();
	}

	bool hit(const Ray& r, Interval ray_t, HitRecord& record) const override {

		bool hit = false;
		double closest_t = ray_t.max_;
		HitRecord temp_record;
		for (const auto& obj : objects_) {
			if (obj->hit(r, Interval(ray_t.min_, closest_t), temp_record)) {

				hit = true;
				closest_t = temp_record.t_;
				record = temp_record;
			}
		}

		return hit;
	}


	AABB getBoundingBox() const override
	{
		return bounding_box_;
	}
};

inline std::shared_ptr<HittableList> Box(const vec3& a, const vec3& b, std::shared_ptr<Material> material)
{
	auto sides = std::make_shared<HittableList>();

	auto min = vec3(std::fmin(a.x, b.x), std::fmin(a.y, b.y), std::fmin(a.z, b.z));
	auto max = vec3(std::fmax(a.x, b.x), std::fmax(a.y, b.y), std::fmax(a.z, b.z));

	auto dx = vec3(max.x - min.x, 0, 0);
	auto dy = vec3(0, max.y - min.y, 0);
	auto dz = vec3(0, 0, max.z - min.z);

	sides->add(std::make_shared<Quad>(vec3(min.x, min.y, max.z), dx, dy, material)); // front
	sides->add(std::make_shared<Quad>(vec3(max.x, min.y, max.z), -dz, dy, material)); // right
	sides->add(std::make_shared<Quad>(vec3(max.x, min.y, min.z), -dx, dy, material)); // back
	sides->add(std::make_shared<Quad>(vec3(min.x, min.y, min.z), dz, dy, material)); // left
	sides->add(std::make_shared<Quad>(vec3(min.x, max.y, max.z), dx, -dz, material)); // top
	sides->add(std::make_shared<Quad>(vec3(min.x, min.y, min.z), dx, dz, material)); // bottom

	return sides;
}