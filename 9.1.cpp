#include "h.h"

#include <algorithm>
#include <cstdint>
#include <fstream>
#include <functional>
#include <iostream>
#include <numeric>
#include <ranges>
#include <span>
#include <unordered_map>
#include <vector>

namespace
{

using namespace std::string_view_literals;

std::vector<std::int64_t> parse_line(std::string_view line)
{
	std::vector<std::int64_t> values;
	for (const auto value : std::views::split(line, ' '))
	{
		char * end;
		const auto number = std::strtoull(value.begin(), &end, 10);
		values.push_back(number);
	}
	return values;
}

std::int64_t process_line(std::string_view line)
{
	std::vector<std::vector<std::int64_t>> diffs{parse_line(line)};
	do
	{
		const auto & back = diffs.back();
		std::vector<std::int64_t> diff;
		for (std::size_t i = 1; i < back.size(); ++i)
		{
			diff.push_back(back[i] - back[i - 1]);
		}
		diffs.push_back(std::move(diff));
	}
	while (!std::ranges::all_of(diffs.back(), std::bind_front(std::equal_to<std::int64_t>(), 0)));

	for (const auto & diff : diffs)
	{
		for (const auto & value : diff)
		{
			std::cout << value << " ";
		}
		std::cout << "\n";
	}
	std::cout << '\n';

	std::int64_t result = 0;
	for (const auto & diff : diffs | std::views::reverse)
	{
		if (!diff.empty())
		{
			result += diff.back();
		}
	}
	return result;
}

} // namespace

int task_9_1(std::string_view input_file)
{
	std::ifstream input(input_file.data());

	std::int64_t result = 0;
	std::string line;
	while (!input.bad() && !input.eof())
	{
		std::getline(input, line);
		result += process_line(line);
	}
	std::cout << result << '\n';

	return 0;
}
