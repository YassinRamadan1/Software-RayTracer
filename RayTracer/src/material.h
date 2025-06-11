#pragma once

#include "hittable.h"
#include "ray.h"
#include "texture.h"

class Material 
{
public:
		virtual ~Material() = default;

		virtual Color emit(double u, double v, const vec3& point) const 
		{
			return Color(0, 0, 0);
		}
		virtual bool scatter(const Ray& r_in, const HitRecord& record, Color& attenuation, Ray& scattered) const 
		{

			return false;
		}
};

class Lambertian : public Material
{
	std::shared_ptr<Texture> texture_;
public:

	Lambertian(std::shared_ptr<Texture> texture) : texture_(texture) {}

	Lambertian(const Color& albedo) : texture_(std::make_shared<SolidTexture>(albedo)) {}


	bool scatter(const Ray& r_in, const HitRecord& record, Color& attenuation, Ray& scattered) const override {

		vec3 scatter_direction = record.normal_ + Vec3::randomUnitVector();
		
		if(scatter_direction.nearZero())
			scatter_direction = record.normal_;

		scattered = Ray(record.intersection_point_, scatter_direction, r_in.time_);
		attenuation = texture_->getValue(record.u_, record.v_, record.intersection_point_);

		return true;
	}
};

class Metal : public Material
{
	double fuzziness_;
	Color albedo_;
public:
	
	Metal(const Color& albedo, double fuzziness = 0.0f) : albedo_(albedo), fuzziness_(fuzziness) {}

	bool scatter(const Ray& r_in, const HitRecord& record, Color& attenuation, Ray& scattered) const override {

		vec3 scatter_direction = reflect(r_in.dir_, record.normal_);

		scatter_direction = normalize(scatter_direction) + fuzziness_ * Vec3::randomUnitVector();
		scattered = Ray(record.intersection_point_, scatter_direction, r_in.time_);
		attenuation = albedo_;

		return dot(scatter_direction, record.normal_) > 0;
	}
};

class Dielectric : public Material
{
	double refractive_index_;

public:

	Dielectric(double refractive_index) : refractive_index_(refractive_index) {}

	bool scatter(const Ray& r_in, const HitRecord& record, Color& attenuation, Ray& scattered) const override {

		attenuation = Color(1.0f, 1.0f, 1.0f);
		vec3 scatter_direction;
		vec3 unit_direction = normalize(r_in.dir_);
		double refraction_ratio = record.front_face_ ? (1.0f / refractive_index_) : refractive_index_;

		double cos_theta = fmin(dot(-unit_direction, record.normal_), 1.0f);
		bool cannot_refract = (refraction_ratio * sqrt(1 - cos_theta * cos_theta) > 1.0f);

		if(cannot_refract || reflectance(cos_theta, refraction_ratio) > RandomDouble())
			scatter_direction = reflect(unit_direction, record.normal_); 		
		else
			scatter_direction = refract(unit_direction, record.normal_, refraction_ratio);

		scattered = Ray(record.intersection_point_, scatter_direction, r_in.time_);

		return true;
	}
	
private:

	static double reflectance(double cosine, double refractive_index)
	{
		double r0 = (1 - refractive_index) / (1 + refractive_index);
		double v = 1 - cosine;
		r0 *= r0;
		return r0 + (1 - r0) * v * v * v * v * v;
	}
};

class DiffuseLight : public Material
{
	std::shared_ptr<Texture> texture_;
public:

	DiffuseLight(std::shared_ptr<Texture> texture) : texture_(texture) {}

	DiffuseLight(const vec3& color) : texture_(std::make_shared<SolidTexture>(color)) {}

	Color emit(double u, double v, const vec3& point) const override
	{
		return texture_->getValue(u, v, point);
	}

};