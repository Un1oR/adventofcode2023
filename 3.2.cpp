#include "h.h"

#include <array>
#include <fstream>
#include <iostream>
#include <unordered_map>

namespace
{

constexpr std::size_t max_part_number_len = 3;
constexpr std::size_t max_adjacent_parts = 3 * (max_part_number_len + 2) + 2;

std::array<int, max_adjacent_parts> get_gear_idx(
	std::size_t cur_line,
	std::string_view line1,
	std::string_view line2,
	std::string_view line3,
	std::size_t number_start,
	std::size_t number_end)
{
	auto to_gear_idx = [row_len = line1.size()](std::size_t i, std::size_t j) -> int
	{
		return j * row_len + i + 1;
	};

	std::array<int, max_adjacent_parts> result{};
	std::size_t idx = 0;
	if (number_start)
	{
		if (line1[number_start - 1] == '*')
		{
			result[idx++] = to_gear_idx(cur_line - 1, number_start - 1);
		}
		if (line2[number_start - 1] == '*')
		{
			result[idx++] = to_gear_idx(cur_line, number_start - 1);
		}
		if (line3[number_start - 1] == '*')
		{
			result[idx++] = to_gear_idx(cur_line + 1, number_start - 1);
		}
	}
	for (std::size_t i = number_start; i <= number_end; ++i)
	{
		if (line1[i] == '*')
		{
			result[idx++] = to_gear_idx(cur_line - 1, i);
		}
		if (line3[i] == '*')
		{
			result[idx++] = to_gear_idx(cur_line + 1, i);
		}
	}
	if (number_end != line1.size() - 1)
	{
		if (line1[number_end + 1] == '*')
		{
			result[idx++] = to_gear_idx(cur_line - 1, number_end + 1);
		}
		if (line2[number_end + 1] == '*')
		{
			result[idx++] = to_gear_idx(cur_line, number_end + 1);
		}
		if (line3[number_end + 1] == '*')
		{
			result[idx++] = to_gear_idx(cur_line + 1, number_end + 1);
		}
	}
	return result;
}

void save_part_number(std::unordered_map<int, std::array<int, 3>>::iterator it, unsigned number)
{
	auto & [key, value] = *it;
	if (value[0] == -1)
	{
		value[0] = number;
		return;
	}
	if (value[1] == -1)
	{
		value[1] = number;
		return;
	}
	if (value[2] == -1)
	{
		value[2] = number;
		return;
	}
}

void process_lines(
	std::unordered_map<int, std::array<int, 3>> & gear_ratios,
	std::size_t cur_line,
	std::string_view line1,
	std::string_view line2,
	std::string_view line3)
{
	std::size_t i = 0;
	std::array<int, 3> empty{-1, -1, -1};
	while (true)
	{
		std::size_t number_start = line2.find_first_of("0123456789", i);
		if (number_start == std::string_view::npos)
		{
			break;
		}
		i = number_start;
		while (std::isdigit(line2[i]))
		{
			++i;
		};
		unsigned number = atoi(line2.substr(number_start, i - number_start).data());
		const auto gear_indices = get_gear_idx(cur_line, line1, line2, line3, number_start, i - 1);
		for (const auto gear_idx : gear_indices)
		{
			if (gear_idx)
			{
				auto [it, inserted] = gear_ratios.insert(std::pair(gear_idx, empty));
				save_part_number(it, number);
			}
		}
	}
}

} // namespace

int task_3_2(std::string_view input_file)
{
	std::ifstream input(input_file.data());
	std::string line1, line2, line3;

	std::getline(input, line2);
	line1 = std::string(line2.size(), '.');

	std::size_t cur_line = 1;
	std::unordered_map<int, std::array<int, 3>> gear_ratios;
	while (!input.bad() && !input.eof())
	{
		std::getline(input, line3);
		process_lines(gear_ratios, cur_line++, line1, line2, line3);
		line1 = std::move(line2);
		line2 = std::move(line3);
	}
	line3 = std::string(line2.size(), '.');
	process_lines(gear_ratios, cur_line, line1, line2, line3);

	unsigned result = 0;
	for (const auto & [key, value] : gear_ratios)
	{
		if (value[0] != -1 && value[1] != -1 && value[2] == -1)
		{
			result += value[0] * value[1];
		}
	}

	std::cout << result << '\n';
	return 0;
}
