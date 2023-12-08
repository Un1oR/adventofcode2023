#include "h.h"

#include <cmath>
#include <fstream>
#include <iostream>
#include <vector>

namespace
{

using namespace std::string_view_literals;

struct Race
{
	unsigned time = 0;
	unsigned distance = 0;
};

struct Races
{
	std::vector<unsigned> times;
	std::vector<unsigned> distances;

	std::size_t size() const
	{
		return times.size();
	}

	Race operator[](std::size_t idx) const
	{
		return Race{times[idx], distances[idx]};
	}
};

std::vector<unsigned> parse_data(std::string_view line)
{
	std::vector<unsigned> result;
	line = line.substr(line.find_first_of(':') + 1);
	while (true)
	{
		while (line[0] == ' ')
		{
			line = line.substr(1);
		}
		auto number_end = line.find_first_of(' ');
		unsigned number = atoi(line.substr(0, number_end).data());
		result.push_back(number);
		if (number_end == std::string_view::npos)
		{
			break;
		}
		line = line.substr(number_end);
	}
	return result;
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
	unsigned t1 = ceil(0.5 * (race.time - sqrt(race.time * race.time - 4 * race.distance)));
	unsigned t2 = trunc(0.5 * (race.time + sqrt(race.time * race.time - 4 * race.distance)));
	unsigned t1_distance = t1 * (race.time - t1);
	unsigned t2_distance = t2 * (race.time - t2);
	if (t1_distance == race.distance)
	{
		++t1;
	}
	if (t2_distance == race.distance)
	{
		--t2;
	}
	return t2 - t1 + 1;
}

} // namespace

int task_6_1(std::string_view input_file)
{
	std::ifstream input(input_file.data());

	std::string times;
	std::getline(input, times);
	std::string distances;
	std::getline(input, distances);
	Races races{parse_data(times), parse_data(distances)};
	unsigned result = 1;
	for (std::size_t i = 0; i < races.size(); ++i)
	{
		result *= calc_one(races[i]);
	}
	std::cout << result << '\n';

	return 0;
}
