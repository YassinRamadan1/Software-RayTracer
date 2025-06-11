#pragma once

#include "utility.h"
#include "hittable.h"
#include "material.h"

class Camera {

private:

	vec3 camera_position_,
		delta_right_,
		delta_up_,
		pixel00_loc_, // we start from the bottom left of the viewport
		defocus_disk_right_,
		defocus_disk_up_;
	std::vector<unsigned int> x_iterator_,
		y_iterator_;

	Color rayColor(const Ray& r, int cur_depth, const Hittable& object) const 
	{
		if(cur_depth <= 0)
			return Color(0, 0, 0);

		HitRecord record;
		if (!object.hit(r, Interval(0.001, Infinity), record))
			return background_color_;
		Ray scattered;
		Color attenuation;
		Color emissive_color = record.material_->emit(record.u_, record.v_, record.intersection_point_);

		if (!record.material_->scatter(r, record, attenuation, scattered))
			return emissive_color;

		Color scattering_color = attenuation * rayColor(scattered, cur_depth - 1, object);

		return emissive_color + scattering_color;
	}

	Ray getRay(int i, int j)
	{
		vec3 offset = sampleSquare(),
			pixelij_loc = pixel00_loc_ + (i + offset.x) * delta_right_ + (j + offset.y) * delta_up_,
			ray_origin = (defocus_angle_ <= 0) ? camera_position_ : defocusDiskSample(),
			ray_direction = pixelij_loc - ray_origin;
		double ray_time = RandomDouble();
		
		return Ray(ray_origin, ray_direction, ray_time);
	}

	vec3 sampleSquare() const  {

		return vec3(RandomDouble() - 0.5, RandomDouble() - 0.5, 0);
	}

	vec3 defocusDiskSample() const {
	
		vec3 disk_sample = Vec3::randomInUnitDisk();

		return camera_position_ + disk_sample.x * defocus_disk_right_ + disk_sample.y * defocus_disk_up_;
	}

public:

	int image_width_ = 100,
		image_height_,
		samples_per_pixel_ = 50,
		max_depth_ = 50;
	double aspect_ratio_ = 1.0f,
		vertical_fov_ = 90.0f, // in degrees
		defocus_angle_ = 0.0f, // in degrees
		focus_distance_ = 10.0f;
	vec3 look_from_ = vec3(0, 0, 0),
		look_at_ = vec3(0, 0, -1),
		world_up_ = vec3(0, 1, 0);
	vec3 background_color_;
	TGAImage* image_ = nullptr;

	void init()
	{
		image_height_ = image_width_ / aspect_ratio_;
			image_height_ = (image_height_ < 1) ? 1 : image_height_,
			y_iterator_.clear(),
			y_iterator_.resize(image_height_),
			x_iterator_.clear(),
			x_iterator_.resize(image_width_);

			for (unsigned int i = 0; i < image_height_; i++)
				y_iterator_[i] = i;
			for (unsigned int i = 0; i < image_width_; i++)
				x_iterator_[i] = i;

		double h = tan(DegreesToRadians(vertical_fov_ / 2.0f)) * focus_distance_,
			disk_radius = focus_distance_ * tan(DegreesToRadians(defocus_angle_ / 2.0f)),
			view_height = 2.0f * h,
			view_width = view_height * (float(image_width_) / image_height_);

		vec3 front = normalize(look_from_ - look_at_),
			right = normalize(cross(world_up_, front)),
			up = cross(front, right),
			viewport_right_ = view_width * right,
			viewport_up_ = view_height * up;

		defocus_disk_right_ = disk_radius * right,
			defocus_disk_up_ = disk_radius * up;

		camera_position_ = look_from_,
		delta_right_ = (1.0f / image_width_) * viewport_right_,
		delta_up_ = (1.0f / image_height_) * viewport_up_,
		// we start from the bottom left of the viewport
		pixel00_loc_ = camera_position_ - (focus_distance_ * front) + 0.5f * (-viewport_up_ - viewport_right_ + delta_right_ + delta_up_);
	}

	void render(const Hittable& world) 
	{
#define MULTI_THREADS 0

#if MULTI_THREADS

		std::for_each(std::execution::par, y_iterator_.begin(), y_iterator_.end(),
			[this, &world](unsigned int j)
			{
				for (unsigned int i = 0; i < image_width_; i++)
				{
					Color c(0.0, 0.0, 0.0);

					for (int sample = 0; sample < samples_per_pixel_; sample++) {

						Ray r = getRay(i, j);

						c += rayColor(r, max_depth_, world);
					}

					c *= (1.0f / samples_per_pixel_);
					image_->set(i, j, toTGAColor(linearToGamma(c)));
				}
			});
#else
		for (unsigned int j = 0; j < image_height_; j++)
			for (unsigned int i = 0; i < image_width_; i++)
			{
				Color c(0.0, 0.0, 0.0);
				
				for (int sample = 0; sample < samples_per_pixel_; sample++) {

					Ray r = getRay(i, j);

					c += rayColor(r, max_depth_, world);
				}
				
				c *= (1.0f / samples_per_pixel_);
				image_->set(i, j, toTGAColor(linearToGamma(c)));
			}
#endif
		image_->write_tga_file("Export/CornellBox.tga");
	}
};