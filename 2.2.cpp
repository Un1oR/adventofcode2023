#include "h.h"

#include <array>
#include <cassert>
#include <fstream>
#include <iostream>

namespace
{

enum class Color
{
	Red = 0,
	Green = 1,
	Blue = 2,
};

using namespace std::string_view_literals;

std::pair<Color, unsigned> parse_count(std::string_view str)
{
	// strip
	while (str[0] == ' ')
	{
		str = str.substr(1);
	}
	// count
	std::size_t digit_end = 0;
	while (std::isdigit(str[digit_end]))
	{
		digit_end++;
	}
	unsigned count = atoi(str.substr(0, digit_end).data());
	// color
	str = str.substr(digit_end + 1);
	if (str[0] == 'r')
	{
		return {Color::Red, count};
	}
	if (str[0] == 'g')
	{
		return {Color::Green, count};
	}
	assert(str[0] == 'b');
	return {Color::Blue, count};
}

std::size_t to_idx(Color color)
{
	return static_cast<std::size_t>(color);
}

std::array<unsigned, 3> get_set_counts(std::string_view str)
{
	std::array<unsigned, 3> result = {0, 0, 0};
	std::size_t comma_idx = 0;
	while (comma_idx != std::string_view::npos)
	{
		comma_idx = str.find_first_of(',');
		auto [color, count] = parse_count(str.substr(0, comma_idx));
		result[to_idx(color)] = count;
		str = str.substr(comma_idx + 1);
	}
	return result;
}

unsigned process_line(std::string_view line)
{
	line = line.substr(line.find_first_of(':') + 1);

	std::size_t semicolon_idx = 0;
	std::array<unsigned, 3> max_counts = {0, 0, 0};
	while (semicolon_idx != std::string_view::npos)
	{
		semicolon_idx = line.find_first_of(';');
		auto counts = get_set_counts(line.substr(0, semicolon_idx));
		for (std::size_t i = 0; i < 3; ++i)
		{
			max_counts[i] = std::max(max_counts[i], counts[i]);
		}
		line = line.substr(semicolon_idx + 1);
	}
	return max_counts[0] * max_counts[1] * max_counts[2];
}

} // namespace

int task_2_2(std::string_view input_file)
{
	std::ifstream input(input_file.data());
	std::string line;
	unsigned result = 0;
	while (!input.bad() && !input.eof())
	{
		std::getline(input, line);
		result += process_line(line);
	}
	std::cout << result << '\n';
	return 0;
}
