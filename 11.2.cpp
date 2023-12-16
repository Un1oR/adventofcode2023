#include "h.h"

#include <cmath>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <unordered_set>
#include <vector>

namespace
{

using namespace std::string_view_literals;

struct Start
{
	std::int64_t x = 0;
	std::int64_t y = 0;
};

bool process_line(
	std::string_view line,
	std::size_t i,
	std::vector<Start> & stars,
	std::unordered_set<std::int64_t> & columns_with_stars)
{
	bool empty = true;
	for (std::size_t j = 0; j < line.size(); ++j)
	{
		if (line[j] == '#')
		{
			stars.emplace_back(j, i);
			columns_with_stars.insert(j);
			empty = false;
		}
	}
	return empty;
}

} // namespace

int task_11_2(std::string_view input_file)
{
	std::ifstream input(input_file.data());

	std::string line;
	std::int64_t current_line = 0;
	std::vector<Start> stars;
	std::unordered_set<std::int64_t> columns_with_stars;
	std::vector<std::int64_t> empty_lines_before = {0};
	while (!input.bad() && !input.eof())
	{
		std::getline(input, line);
		if (process_line(line, current_line++, stars, columns_with_stars))
		{
			empty_lines_before.push_back(empty_lines_before.back() + 1);
		}
		else
		{
			empty_lines_before.push_back(empty_lines_before.back());
		}
	}
	std::vector<std::int64_t> empty_columns_before = {0};
	empty_lines_before.reserve(line.size() - columns_with_stars.size());
	for (std::size_t i = 0; i < line.size(); ++i)
	{
		if (columns_with_stars.contains(i))
		{
			empty_columns_before.push_back(empty_columns_before.back());
		}
		else
		{
			empty_columns_before.push_back(empty_columns_before.back() + 1);
		}
	}

	std::int64_t result = 0;
	constexpr std::int64_t multiplier = 1000000;
	for (std::size_t i = 0; i < stars.size(); ++i)
	{
		for (std::size_t j = i + 1; j < stars.size(); ++j)
		{
			const auto start1 = stars[i];
			const auto start2 = stars[j];
			auto tmp1 = std::abs(start2.x - start1.x);
			auto tmp2 = std::abs(start2.y - start1.y);
			auto tmp3 = (multiplier - 1) * std::abs(empty_lines_before[start2.y] - empty_lines_before[start1.y]);
			auto tmp4 = (multiplier - 1) * std::abs(empty_columns_before[start2.x] - empty_columns_before[start1.x]);
			auto distance = tmp1 + tmp2 + tmp3 + tmp4;
			std::cout << "Distance between ";
			std::cout << i << " (" << stars[i].y << ", " << stars[i].x << ")";
			std::cout << " and " << j << " (" << stars[j].y << ", " << stars[j].x << ")";
			std::cout << " = " << tmp1 << " + " << tmp2 << " + " << tmp3 << " + " << tmp4 << " = ";
			std::cout << distance << '\n';
			result += distance;
		}
	}

	std::cout << result << '\n';

	return 0;
}
