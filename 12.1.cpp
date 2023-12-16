#include "h.h"

#include <algorithm>
#include <cstdint>
#include <format>
#include <fstream>
#include <functional>
#include <iostream>
#include <ranges>
#include <vector>

namespace
{

using namespace std::string_view_literals;

unsigned count_matches(
	std::string_view line,
	int n,
	std::span<int> numbers,
	std::string in_debug,
	std::string_view map,
	std::string_view number_str)
{
	unsigned result = 0;
	std::size_t first_sharp = line.find_first_of('#');
	int end = int(line.size()) - n + 1;
	if (first_sharp != std::string_view::npos)
	{
		end = std::min(int(end), int(first_sharp + 1));
	}
	for (int i = 0; i < end; ++i)
	{
		std::string debug = in_debug;
		std::string_view substr = line.substr(i);
		if (std::ranges::all_of(
				substr.substr(0, n),
				[](const char ch)
				{
					return ch == '#' || ch == '?';
				}))
		{
			for (int j = 0; j < i; ++j)
			{
				debug += '.';
			}
			for (int j = 0; j < n; ++j)
			{
				debug += '#';
			}

			if (!numbers.empty())
			{
				if ((int(substr.size()) >= n + 2) && (substr[n] == '.' || substr[n] == '?'))
				{
					result += count_matches(
						substr.substr(n + 1),
						numbers[0],
						numbers.subspan(1),
						debug + ".",
						map,
						number_str);
				}
			}
			else
			{
				if (substr.find_first_of('#', n) == std::string_view::npos)
				{
					result += 1;
					debug.resize(map.size(), '.');
					std::cout << map << ' ' << number_str << '\n' << debug << "\n\n";
				}
			}
		}
	}
	return result;
}

unsigned process_line(std::string_view line)
{
	std::size_t space = line.find_first_of(' ');
	std::string_view map = line.substr(0, space);
	std::string_view numbers_str = line.substr(space + 1);
	auto r = //
		numbers_str //
		| std::views::split(',')
		| std::views::transform(
			[](auto subrange) -> int
			{
				return std::atoi(subrange.begin());
			});
	std::vector<int> numbers{r.begin(), r.end()};

	std::cout << "\n";
	unsigned result = count_matches(map, numbers[0], std::span(numbers).subspan(1), "", map, numbers_str);
	std::cout << result << "\n====\n";
	return result;
}

} // namespace

int task_12_1(std::string_view input_file)
{
	std::ifstream input(input_file.data());

	std::string line;
	unsigned result = 0;
	while (!input.bad() && !input.eof())
	{
		std::getline(input, line);
		if (!line.starts_with("//"))
		{
			result += process_line(line);
		}
	}
	std::cout << result << '\n';

	return 0;
}
