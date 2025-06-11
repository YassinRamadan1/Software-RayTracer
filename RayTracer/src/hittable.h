#pragma once
#include "ray.h"
#include "interval.h"
#include "utility.h"
#include "aabb.h"

class Material;

class HitRecord {

public:
	bool front_face_;
	double t_;
	double u_, v_;
	vec3 intersection_point_;
	vec3 normal_;
	std::shared_ptr<Material> material_;

	void setNormal(const Ray& r, vec3 outward_normal) {

		front_face_ = dot(outward_normal, r.dir_) < 0;

		normal_ = front_face_ ? outward_normal : -outward_normal;
	}
};

class Hittable {

public:
	
	virtual ~Hittable() = default;

	virtual bool hit(const Ray& r, Interval ray_t, HitRecord& rec) const = 0;

	virtual AABB getBoundingBox() const = 0;
};

class Sphere : public Hittable {

private:

	Ray center_;
	double radius_;
	std::shared_ptr<Material> material_;
	AABB bounding_box_;
public:

	Sphere(vec3 center, double radius, std::shared_ptr<Material> material) : center_(center, vec3(0, 0, 0)), radius_(std::fmax(0, radius)), material_(material)
	{
		vec3 radius_vector(radius_, radius_, radius_);
		bounding_box_ = AABB(center_.orig_ - radius_vector, center_.orig_ + radius_vector);
	}

	// moving sphere from starting_center to final_center over time period of 1
	Sphere(vec3 starting_center, vec3 final_center, double radius, std::shared_ptr<Material> material) : center_(starting_center, final_center - starting_center), radius_(std::fmax(0, radius)), material_(material)
	{
		vec3 radius_vector(radius_, radius_, radius_);
		AABB bounding_box1(center_.At(0) - radius_vector, center_.At(0) + radius_vector),
		bounding_box2(center_.At(1) - radius_vector, center_.At(1) + radius_vector);

		bounding_box_ = AABB(bounding_box1, bounding_box2);
	}

	static void getSphereUV(vec3& point, double& u, double& v)
	{
		// we assume that the given point is on the unit sphere and u, v in range [0, 1] 
		// theta starts from -Y axis in range [0, Pi]
		// phi starts from -X axis towards +Z axis in range [0, 2 * Pi]

		double theta = acos(-point.y), phi = atan2(-point.z, point.x) + Pi;
		u = phi / (2 * Pi), v = theta / Pi;
	}

	bool hit(const Ray& r, Interval ray_t, HitRecord& record) const override
	{
		// knowing where is the sphere at the time of intersection
		vec3 current_center = center_.At(r.time_);

		double a = r.dir_.length2(), h = dot(r.dir_, current_center - r.orig_),
			c = (current_center - r.orig_).length2() - radius_ * radius_, v = h * h - a * c;

		if (v < 0.0f)
			return false;
		double ans = (h - std::sqrt(v)) / a;
		if (!ray_t.surrounds(ans)) {
			ans = (h + std::sqrt(v)) / a;
			if (!ray_t.surrounds(ans))
				return false;
		}

		record.t_ = ans;
		record.intersection_point_ = r.At(ans);
		vec3 normal = (record.intersection_point_ - current_center) / radius_;
		record.setNormal(r, normal);
		record.material_ = material_;
		// normal is the representation of the intersection point but on the unit sphere
		getSphereUV(normal, record.u_, record.v_);
		return true;
	}

	AABB getBoundingBox() const override
	{
		return bounding_box_;
	}
};

class Quad : public Hittable
{

	vec3 corner_;
	vec3 u_, v_;
	vec3 normal_;
	vec3 w_; // it helps with testing hits
	std::shared_ptr<Material> material_;
	AABB bounding_box_;

	void setBoundingBox()
	{
		bounding_box_ = AABB(corner_, corner_ + u_ + v_);
		bounding_box_ = AABB(bounding_box_, AABB(corner_ + u_, corner_ + v_));
	}

public:

	Quad(const vec3& corner, const vec3& u, const vec3& v, std::shared_ptr<Material> material) : corner_(corner), u_(u), v_(v), material_(material)
	{
		vec3 normal = cross(u_, v_);
		normal_ = normalize(normal);
		w_ = normal / dot(normal, normal);
		setBoundingBox();
	}

	bool hit(const Ray& r, Interval ray_t, HitRecord& rec) const override
	{
		double denominator = dot(normal_, r.dir_);
		if (fabs(denominator) < 1e-8)
			return false;

		double t = dot(normal_, corner_ - r.orig_) / denominator;

		if (!ray_t.contains(t))
			return false;
		vec3 intersection_vector = r.At(t) - corner_;
		double alpha = dot(w_, cross(intersection_vector, v_));
		double beta = dot(w_, cross(u_, intersection_vector));

		if (!isInside(alpha, beta, rec))
			return false;

		rec.t_ = t;
		rec.intersection_point_ = intersection_vector + corner_;
		rec.material_ = material_;
		rec.setNormal(r, normal_);

		return true;
	}

	bool isInside(double alpha, double beta, HitRecord& rec) const
	{
		Interval unit_interval(0, 1);
		if (!unit_interval.contains(alpha) || !unit_interval.contains(beta))
			return false;

		rec.u_ = alpha;
		rec.v_ = beta;
		return true;
	}

	AABB getBoundingBox() const override
	{
		return bounding_box_;
	}
};

class Translate : public Hittable
{
	std::shared_ptr<Hittable> object_;
	vec3 translation_;
	AABB bounding_box_;

public:

	Translate(std::shared_ptr<Hittable> object, vec3 translation) : object_(object), translation_(translation)
	{
		bounding_box_ = object->getBoundingBox() + translation_;
	}

	bool hit(const Ray& r, Interval ray_t, HitRecord& rec) const override
	{
		Ray displaced_ray(r.orig_ - translation_, r.dir_, r.time_);

		if (!object_->hit(displaced_ray, ray_t, rec))
			return false;
		rec.intersection_point_ += translation_;
		return true;
	}

	AABB getBoundingBox() const override
	{
		return bounding_box_;
	}
};

class RotateY : public Hittable
{
	std::shared_ptr<Hittable> object_;
	double cosine_theta_;
	double sine_theta_;
	AABB bounding_box_;

public:

	RotateY(std::shared_ptr<Hittable> object, double angle) : object_(object)
	{
		double radians_angle = DegreesToRadians(angle);
		cosine_theta_ = cos(radians_angle);
		sine_theta_ = sin(radians_angle);

		bounding_box_ = object->getBoundingBox();
		vec3 mini(Infinity, Infinity, Infinity);
		vec3 maxi(-Infinity, -Infinity, -Infinity);

		for (int i = 0; i < 2; i++)
			for (int j = 0; j < 2; j++)
				for (int k = 0; k < 2; k++)
				{
					vec3 point(i * bounding_box_.x_.min_ + (i - 1) * bounding_box_.x_.max_,
						j * bounding_box_.y_.min_ + (j - 1) * bounding_box_.y_.max_, k * bounding_box_.z_.min_ + (k - 1) * bounding_box_.z_.max_);

					double temp = cosine_theta_ * point.x + sine_theta_ * point.z;
					point.z = -sine_theta_ * point.x + cosine_theta_ * point.z;
					point.x = temp;

					for (int i = 0; i < 3; i++)
					{
						mini.data[i] = min(mini.data[i], point.data[i]);
						maxi.data[i] = min(maxi.data[i], point.data[i]);
					}
				}
		bounding_box_ = AABB(mini, maxi);
	}

	bool hit(const Ray& r, Interval ray_t, HitRecord& rec) const override
	{
		vec3 new_origin(cosine_theta_ * r.orig_.x - sine_theta_ * r.orig_.z, r.orig_.y, sine_theta_ * r.orig_.x + cosine_theta_ * r.orig_.z);

		vec3 new_direction(cosine_theta_ * r.dir_.x - sine_theta_ * r.dir_.z, r.dir_.y, sine_theta_ * r.dir_.x + cosine_theta_ * r.dir_.z);
		
		if (!object_->hit(Ray(new_origin, new_direction, r.time_), ray_t, rec))
			return false;
		
		rec.intersection_point_ = vec3(cosine_theta_ * rec.intersection_point_.x + sine_theta_ * rec.intersection_point_.z, rec.intersection_point_.y, -sine_theta_ * rec.intersection_point_.x + cosine_theta_ * rec.intersection_point_.z);
	
		rec.normal_ = vec3(cosine_theta_ * rec.normal_.x + sine_theta_ * rec.normal_.z, rec.normal_.y, -sine_theta_ * rec.normal_.x + cosine_theta_ * rec.normal_.z);
		
		return true;
	}

	AABB getBoundingBox() const override
	{
		return bounding_box_;
	}
};