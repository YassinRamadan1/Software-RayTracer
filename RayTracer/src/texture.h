#pragma once

#include "vec.h"
#include "noise.h"

class Texture
{
public:

	virtual ~Texture() = default;
	virtual Color getValue(double u, double v, const vec3& point) const = 0;
};

class SolidTexture : public Texture
{
	Color albedo_;

public:

	SolidTexture(const Color& albedo) : albedo_(albedo) {}

	SolidTexture(double red, double green, double blue) : SolidTexture(Color(red, green, blue)) {}

	Color getValue(double u, double v, const vec3& point) const override
	{
		return albedo_;
	}
};

class CheckerTexture : public Texture
{

	double inverse_scale_;
	std::shared_ptr<Texture> even_, odd_;

public:

	CheckerTexture(double scale, std::shared_ptr<Texture> even, std::shared_ptr<Texture> odd)
		: inverse_scale_(1 / scale), even_(even), odd_(odd) {}

	CheckerTexture(double scale, const Color& c1, const Color& c2)
		: CheckerTexture(scale, std::make_shared<SolidTexture>(c1), std::make_shared<SolidTexture>(c2)) {}


	Color getValue(double u, double v, const vec3& point) const override
	{
		vec3 scaled_point = inverse_scale_ * point;
		bool is_odd = int(floor(scaled_point.x) + floor(scaled_point.y) + floor(scaled_point.z)) % 2;
		
		return (is_odd ? odd_->getValue(u, v, point) : even_->getValue(u, v, point));
	}
};

class ImageTexture : public Texture
{

	TGAImage image_;

public:

	ImageTexture(const char* image_path)
	{
		if (!image_.read_tga_file(image_path))
		{
			std::cerr << "Couldn't load the image with path : " << image_path << '\n';
			return;
		}
		for (int i = 0; i < image_.width(); i++)
			for (int j = 0; j < image_.height(); j++)
			{ // converting colors from gamma space to linear space
				Color pixel_color = toColor(image_.get(i, j));
				image_.set(i, j, toTGAColor(gammaToLinear(pixel_color)));
			}
		image_.flip_vertically();
	}

	Color getValue(double u, double v, const vec3& point) const override
	{
		if (!image_.width() || !image_.height())
			return Color(0, 1, 1);

		u = Interval(0, 1).clamp(u), v = Interval(0, 1).clamp(v);
		int i = u * (image_.width() - 1), j = v * (image_.height() - 1);

		return toColor(image_.get(i, j));
	}
};

class NoiseTexture : public Texture
{

	double scale_;
	std::shared_ptr<Noise> noise_;
public:

	NoiseTexture(std::shared_ptr<Noise> noise, double scale) : noise_(noise), scale_(scale) {}

	Color getValue(double u, double v, const vec3& point) const override
	{
		return Color(0.5, 0.5, 0.5) * (1.0 + sin(scale_ * point.x + 10.0 * noise_->getTurbuelence(point, 7)));
	}
};