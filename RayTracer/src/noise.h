#pragma once

#include "utility.h"
#include "vec.h"

class Noise
{

public:

	virtual ~Noise() = default;
	virtual double getValue(const vec3& point) const = 0;
	virtual double getTurbuelence(const vec3& point, int depth) const = 0;

};

class ValueNoise : public Noise
{

	int point_count_ = 256;
	std::vector<double> random_values_;
	std::vector<int> permutation_table_;


	void generatePermutationTable()
	{
		permutation_table_.resize(2 * point_count_);
		for (int i = 0; i < point_count_; i++)
		{
			permutation_table_[i] = i;
		}

		for (int i = 0; i < point_count_; i++)
		{
			int random = RandomInteger(0, point_count_ - 1);

			std::swap(permutation_table_[i], permutation_table_[random]);
		}

		for (int i = 0; i < point_count_; i++)
		{
			permutation_table_[point_count_ + i] = permutation_table_[i];
		}
	}

public:

	ValueNoise(int grid_size) : point_count_(grid_size)
	{
		if ((point_count_ & (point_count_ - 1)) != 0)
		{
			std::cerr << "point_count_ must be a power of 2\n";
		}

		random_values_.resize(point_count_);
		for (int i = 0; i < point_count_; i++)
		{
			random_values_[i] = RandomDouble();
		}

		generatePermutationTable();
	}

	int hash(int x, int y, int z) const
	{
		return permutation_table_[permutation_table_[permutation_table_[x] + y] + z];
	}

	double getValue(const vec3& point) const override
	{
		int x_min = std::floor(point.x);
		int y_min = std::floor(point.y);
		int z_min = std::floor(point.z);

		double tx = point.x - x_min;
		double ty = point.y - y_min;
		double tz = point.z - z_min;

		x_min = x_min & (point_count_ - 1);
		y_min = y_min & (point_count_ - 1);
		z_min = z_min & (point_count_ - 1);
		int x_max = (x_min + 1) & (point_count_ - 1);
		int y_max = (y_min + 1) & (point_count_ - 1);
		int z_max = (z_min + 1) & (point_count_ - 1);

		// smoothing the interpolation parameters
		double u = Smoothstep(0, 1, tx);
		double v = Smoothstep(0, 1, ty);
		double w = Smoothstep(0, 1, tz);

		// first four values
		double value000 = random_values_[hash(x_min, y_min, z_min)];
		double value100 = random_values_[hash(x_max, y_min, z_min)];
		double value010 = random_values_[hash(x_min, y_max, z_min)];
		double value110 = random_values_[hash(x_max, y_max, z_min)];

		double value_00 = Lerp(value000, value100, u);
		double value_10 = Lerp(value010, value110, u);
		double value__0 = Lerp(value_00, value_10, v);

		// second four values
		double value001 = random_values_[hash(x_min, y_min, z_max)];
		double value101 = random_values_[hash(x_max, y_min, z_max)];
		double value011 = random_values_[hash(x_min, y_max, z_max)];
		double value111 = random_values_[hash(x_max, y_max, z_max)];

		double value_01 = Lerp(value001, value101, u);
		double value_11 = Lerp(value011, value111, u);
		double value__1 = Lerp(value_01, value_11, v);

		return Lerp(value__0, value__1, w);
	}

	double getTurbuelence(const vec3& point, int depth) const override
	{
		double accumulation = 0.0f;
		double weight = 1.0f;
		double frequency = 1.0f;
		for (int i = 0; i < depth; i++)
		{
			accumulation += weight * getValue(frequency * point);
			weight *= 0.5;
			frequency *= 2.0;
		}

		return fabs(accumulation);
	}
};

class PerlinNoise : public Noise
{

	int point_count_ = 256;
	std::vector<vec3> random_gradients_;
	std::vector<int> permutation_table_;


	void generatePermutationTable()
	{
		permutation_table_.resize(2 * point_count_);
		for (int i = 0; i < point_count_; i++)
		{
			permutation_table_[i] = i;
		}

		for (int i = 0; i < point_count_; i++)
		{
			int random = RandomInteger(0, point_count_ - 1);

			std::swap(permutation_table_[i], permutation_table_[random]);
		}

		for (int i = 0; i < point_count_; i++)
		{
			permutation_table_[point_count_ + i] = permutation_table_[i];
		}
	}

public:

	PerlinNoise(int grid_size) : point_count_(grid_size)
	{
		if ((point_count_ & (point_count_ - 1)) != 0)
		{
			std::cerr << "point_count_ must be a power of 2\n";
		}

		random_gradients_.resize(point_count_);
		for (int i = 0; i < point_count_; i++)
		{
			random_gradients_[i] = normalize(vec3(RandomDouble(-1, 1), RandomDouble(-1, 1), RandomDouble(-1, 1)));
		}

		generatePermutationTable();
	}

	int hash(int x, int y, int z) const
	{
		return permutation_table_[permutation_table_[permutation_table_[x] + y] + z];
	}

	double getValue(const vec3& point) const override
	{
		int x_min = std::floor(point.x);
		int y_min = std::floor(point.y);
		int z_min = std::floor(point.z);
		
		double tx = point.x - x_min;
		double ty = point.y - y_min;
		double tz = point.z - z_min; 
		
		x_min = x_min & (point_count_ - 1);
		y_min = y_min & (point_count_ - 1);
		z_min = z_min & (point_count_ - 1);
		int x_max = (x_min + 1) & (point_count_ - 1);
		int y_max = (y_min + 1) & (point_count_ - 1);
		int z_max = (z_min + 1) & (point_count_ - 1);

		// smoothing the interpolation parameters
		double u = Smootherstep(tx);
		double v = Smootherstep(ty);
		double w = Smootherstep(tz);

		// first four directions
		vec3 direction000 = vec3(tx, ty, tz);
		vec3 direction100 = vec3(tx - 1, ty, tz);
		vec3 direction010 = vec3(tx, ty - 1, tz);
		vec3 direction110 = vec3(tx - 1, ty - 1, tz);

		// first four gradients
		vec3 value000 = random_gradients_[hash(x_min, y_min, z_min)];
		vec3 value100 = random_gradients_[hash(x_max, y_min, z_min)];
		vec3 value010 = random_gradients_[hash(x_min, y_max, z_min)];
		vec3 value110 = random_gradients_[hash(x_max, y_max, z_min)];

		double value_00 = Lerp(dot(value000, direction000), dot(value100, direction100), u);
		double value_10 = Lerp(dot(value010, direction010), dot(value110, direction110), u);
		double value__0 = Lerp(value_00, value_10, v);

		// second four directions
		vec3 direction001 = vec3(tx, ty, tz - 1);
		vec3 direction101 = vec3(tx - 1, ty, tz - 1);
		vec3 direction011 = vec3(tx, ty - 1, tz - 1);
		vec3 direction111 = vec3(tx - 1, ty - 1, tz - 1);

		// second four gradients
		vec3 value001 = random_gradients_[hash(x_min, y_min, z_max)];
		vec3 value101 = random_gradients_[hash(x_max, y_min, z_max)];
		vec3 value011 = random_gradients_[hash(x_min, y_max, z_max)];
		vec3 value111 = random_gradients_[hash(x_max, y_max, z_max)];

		double value_01 = Lerp(dot(value001, direction001) , dot(value101, direction101), u);
		double value_11 = Lerp(dot(value011, direction011), dot(value111, direction111), u);
		double value__1 = Lerp(value_01, value_11, v);

		return Lerp(value__0, value__1, w);
	}

	double getTurbuelence(const vec3& point, int depth) const override
	{
		double accumulation = 0.0f;
		double weight = 1.0f;
		double frequency = 1.0f;
		for (int i = 0; i < depth; i++)
		{
			accumulation += weight * getValue(frequency * point);
			weight *= 0.5;
			frequency *= 2.0;
		}

		return fabs(accumulation);
	}
};