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

struct CacheKey
{
	const char * str;
	std::int64_t n;
	std::span<std::int64_t> numbers;

	constexpr bool operator==(const CacheKey & other) const noexcept
	{
		return str == other.str && numbers.size() == numbers.size();
	}
};

} // namespace

template <>
struct std::hash<CacheKey>
{
	std::size_t operator()(const CacheKey & entry) const noexcept
	{
		std::size_t h1 = std::hash<std::size_t>{}(reinterpret_cast<std::uintptr_t>(entry.str));
		std::size_t h2 = std::hash<std::size_t>{}(entry.numbers.size());
		return h1 ^ (h2 << 1);
	}
};

namespace
{

using Cache = std::unordered_map<CacheKey, std::uint64_t>;

std::uint64_t count_matches(std::string_view line, std::int64_t n, std::span<std::int64_t> numbers, Cache & cache)
{
	CacheKey cache_key{line.data(), n, numbers};
	if (auto cached_result_it = cache.find(cache_key); cached_result_it != cache.cend())
	{
		return cached_result_it->second;
	}
	std::uint64_t result = 0;
	std::size_t first_sharp = line.find_first_of('#');
	std::int64_t end = std::int64_t(line.size()) - n + 1;
	if (first_sharp != std::string_view::npos)
	{
		end = std::min(std::int64_t(end), std::int64_t(first_sharp + 1));
	}
	for (std::int64_t i = 0; i < end; ++i)
	{
		std::string_view substr = line.substr(i);
		if (std::ranges::all_of(
				substr.substr(0, n),
				[](const char ch)
				{
					return ch == '#' || ch == '?';
				}))
		{
			if (!numbers.empty())
			{
				if ((std::int64_t(substr.size()) >= n + 2) && (substr[n] == '.' || substr[n] == '?'))
				{
					result += count_matches(substr.substr(n + 1), numbers[0], numbers.subspan(1), cache);
				}
			}
			else
			{
				if (substr.find_first_of('#', n) == std::string_view::npos)
				{
					result += 1;
				}
			}
		}
	}
	cache[cache_key] = result;
	return result;
}

std::uint64_t process_line(std::string_view line)
{
	std::size_t space = line.find_first_of(' ');
	std::string map = std::string(line.substr(0, space));
	map.reserve(4 + 5 * map.size());
	auto end = map.cend();
	for (std::size_t i = 0; i < 4; ++i)
	{
		map.push_back('?');
		std::copy(map.cbegin(), end, std::back_insert_iterator(map));
	}

	std::string_view numbers_str = line.substr(space + 1);
	auto r = //
		numbers_str //
		| std::views::split(',')
		| std::views::transform(
			[](auto subrange) -> std::int64_t
			{
				return std::atoi(subrange.begin());
			});
	std::vector<std::int64_t> numbers{r.begin(), r.end()};
	numbers.reserve(numbers.size());
	for (std::size_t i = 0; i < 4; ++i)
	{
		std::ranges::copy(r, std::back_insert_iterator(numbers));
	}

	std::cout << "\n";
	Cache cache;
	std::uint64_t result = count_matches(map, numbers[0], std::span(numbers).subspan(1), cache);
	std::cout << "+" << result << "\n\n";
	return result;
}

} // namespace

int task_12_2(std::string_view input_file)
{
	std::ifstream input(input_file.data());

	std::string line;
	unsigned line_number = 0;
	std::uint64_t result = 0;
	while (!input.bad() && !input.eof())
	{
		std::getline(input, line);
		if (!line.starts_with("//"))
		{
			std::cout << "L" << line_number++;
			result += process_line(line);
		}
	}
	std::cout << result << '\n';

	return 0;
}
