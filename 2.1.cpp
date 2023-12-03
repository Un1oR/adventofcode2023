#include "2.1.h"

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
constexpr unsigned total[] = {12, 13, 14};

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

bool check_color(Color color, unsigned count)
{
	return count <= total[static_cast<std::size_t>(color)];
}

bool check_set(std::string_view str)
{
	std::size_t comma_idx = 0;
	while (true)
	{
		comma_idx = str.find_first_of(',');
		if (comma_idx == std::string_view::npos)
		{
			break;
		}
		auto [color, count] = parse_count(str.substr(0, comma_idx));
		if (!check_color(color, count))
		{
			return false;
		}
		str = str.substr(comma_idx + 1);
	}
	auto [color, count] = parse_count(str.substr(0, comma_idx));
	if (!check_color(color, count))
	{
		return false;
	}
	return true;
}

unsigned process_line(std::string_view line)
{
	line = line.substr("Game "sv.size());
	std::size_t colon_idx = line.find_first_of(':') - 1;
	const unsigned game_number = atoi(line.substr(0, colon_idx - 1).data());
	line = line.substr(colon_idx + 2);
	std::size_t semicolon_idx = 0;
	while (true)
	{
		semicolon_idx = line.find_first_of(';');
		if (semicolon_idx == std::string_view::npos)
		{
			break;
		}
		if (!check_set(line.substr(0, semicolon_idx)))
		{
			return 0;
		}
		line = line.substr(semicolon_idx + 1);
	}
	if (!check_set(line.substr(0)))
	{
		return 0;
	}
	return game_number;
}

} // namespace

int task_2_1(std::string_view input_file)
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
