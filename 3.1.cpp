#include "h.h"

#include <fstream>
#include <iostream>

namespace
{

bool is_part_number(
	std::string_view line1,
	std::string_view line2,
	std::string_view line3,
	std::size_t number_start,
	std::size_t number_end)
{
	if (number_start)
	{
		if (line1[number_start - 1] != '.' || line2[number_start - 1] != '.' || line3[number_start - 1] != '.')
		{
			return true;
		}
	}
	for (std::size_t i = number_start; i <= number_end; ++i)
	{
		if (line1[i] != '.' || line3[i] != '.')
		{
			return true;
		}
	}
	if (number_end != line1.size() - 1)
	{
		if (line1[number_end + 1] != '.' || line2[number_end + 1] != '.' || line3[number_end + 1] != '.')
		{
			return true;
		}
	}
	return false;
}

unsigned process_lines(std::string_view line1, std::string_view line2, std::string_view line3)
{
	unsigned result = 0;
	std::size_t i = 0;
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
		if (is_part_number(line1, line2, line3, number_start, i - 1))
		{
			result += number;
		}
	}
	return result;
}

} // namespace

int task_3_1(std::string_view input_file)
{
	std::ifstream input(input_file.data());
	std::string line1, line2, line3;
	unsigned result = 0;

	std::getline(input, line2);
	line1 = std::string(line2.size(), '.');

	while (!input.bad() && !input.eof())
	{
		std::getline(input, line3);
		result += process_lines(line1, line2, line3);
		line1 = std::move(line2);
		line2 = std::move(line3);
	}
	line3 = std::string(line2.size(), '.');
	result += process_lines(line1, line2, line3);

	std::cout << result << '\n';
	return 0;
}
