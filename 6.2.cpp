#include "h.h"

#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <iostream>

namespace
{

using namespace std::string_view_literals;

struct Race
{
	std::uint64_t time = 0;
	std::uint64_t distance = 0;
};

std::uint64_t parse_data(std::string_view line, std::string_view name)
{
	line = line.substr(line.find_first_of(':') + 1);
	std::string number;
	while (true)
	{
		while (line[0] == ' ')
		{
			line = line.substr(1);
		}
		auto number_end = line.find_first_of(' ');
		number += line.substr(0, number_end);
		if (number_end == std::string_view::npos)
		{
			break;
		}
		line = line.substr(number_end);
	}
	std::cout << name << " = " << number << "\n";
	char * end = nullptr;
	return strtoul(number.c_str(), &end, 10);
}

unsigned calc_one(Race race)
{
	//  ^ D
	//  |             /
	//  |            /
	//  |           /
	//  |          /
	//  |         /
	//  |________/
	//  +--------|-----|---> T
	//      t1     t2
	//
	// y = k * x
	// D = t1 * t2
	// T = t1 + t2
	// t2 = T - t1
	// D = t1 * (T - T1)
	// t1^2 - T * t1 + D = 0
	// t1_1 = (T - sqrt(T^2 - 4 * D)) / 2
	// t1_2 = (T + sqrt(T^2 - 4 * D)) / 2
	std::uint64_t T = race.time;
	std::uint64_t D = race.distance;
	std::uint64_t t1 = std::uint64_t(ceil(0.5 * (T - sqrt(T * T - 4 * D))));
	std::uint64_t t2 = std::uint64_t(trunc(0.5 * (T + sqrt(T * T - 4 * D))));
	std::uint64_t t1_distance = t1 * (race.time - t1);
	std::uint64_t t2_distance = t2 * (race.time - t2);
	if (t1_distance == race.distance)
	{
		++t1;
	}
	if (t2_distance == race.distance)
	{
		--t2;
	}
	std::cout << "t1 = " << t1 << '\n';
	std::cout << "t2 = " << t2 << '\n';
	return t2 - t1 + 1;
}

} // namespace

int task_6_2(std::string_view input_file)
{
	std::ifstream input(input_file.data());

	std::string times;
	std::getline(input, times);
	std::string distances;
	std::getline(input, distances);
	Race race{parse_data(times, "time"), parse_data(distances, "distance")};
	std::cout << calc_one(race) << '\n';

	return 0;
}
