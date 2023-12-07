#include "h.h"

#include <fstream>
#include <functional>
#include <iostream>
#include <ranges>
#include <unordered_set>

namespace
{

using namespace std::string_view_literals;

struct MappingPart
{
	unsigned dst_start;
	unsigned src_start;
	unsigned len;

	unsigned dst_end() const
	{
		return dst_start + len;
	}

	unsigned src_end() const
	{
		return src_start + len - 1;
	}
};

MappingPart to_mapping_part(std::string_view str)
{
	std::size_t e = str.find_first_of(' ');
	unsigned dst_start = atoi(str.substr(0, e).data());

	str = str.substr(e + 1);

	e = str.find_first_of(' ');
	unsigned src_start = atoi(str.substr(0, e).data());
	unsigned len = atoi(str.substr(e + 1).data());
	return {dst_start, src_start, len};
}

struct Range
{
	unsigned start;
	unsigned len;
	unsigned end()
	{
		return start + len - 1;
	}
};

class Mapping
{
public:
	Mapping(std::string from, std::string to, std::vector<MappingPart> parts)
		: from_(std::move(from))
		, to_(std::move(to))
		, parts_(std::move(parts))
	{
		std::sort(
			parts_.begin(),
			parts_.end(),
			[](const MappingPart & lhs, const MappingPart & rhs)
			{
				return lhs.src_start < rhs.src_start;
			});
	}

	std::string_view from() const &
	{
		return from_;
	}

	std::string_view to() const
	{
		return to_;
	}

	std::vector<Range> map(Range src) const
	{
		auto it = parts_.begin();
		while (it != parts_.end() && src.start > it->src_end())
		{
			++it;
		}

		if (it == parts_.end())
		{
			return {src};
		}
		if (src.start < it->src_start)
		{
			auto part1 = Range{src.start, it->src_start - src.start + 1};
			auto part2 = Range{it->src_start, src.end() - it->src_start};
			return combine(part1, part2);
		}
		if (src.end() > it->src_end())
		{
			auto part1 = Range{src.start, it->src_end() - src.start};
			auto part2 = Range{it->src_end() + 1, src.end() - it->src_end()};
			return combine(part1, part2);
		}
		return {Range{src.start - it->src_start + it->dst_start, src.len}};
	}

	std::vector<Range> combine(Range part1, Range part2) const
	{
		if (part1.len == 0)
		{
			return {part2};
		}
		if (part2.len == 0)
		{
			return {part1};
		}
		std::vector<Range> result = map(std::move(part1));
		std::vector<Range> part2_mapped = map(std::move(part2));
		std::move(part2_mapped.begin(), part2_mapped.end(), std::back_insert_iterator(result));
		return result;
	}

	void transform(std::vector<Range> & data) const
	{
		std::vector<Range> result;
		for (auto & item : data)
		{
			for (const Range & src : map(item))
			{
				result.push_back(src);
			}
		}
		data = result;
	}

private:
	std::string from_;
	std::string to_;
	std::vector<MappingPart> parts_;
};

void dump(std::string_view type, const std::vector<Range> & values)
{
	std::cout << type << ": ";
	for (const Range & value : values)
	{
		std::cout << value.start << "|" << value.len << " ";
	}
	std::cout << '\n';
}

class Almanac
{
public:
	Almanac(std::vector<Range> seeds, std::vector<Mapping> mappings)
		: seeds_(std::move(seeds))
	{
		for (auto & mapping : mappings)
		{
			auto from = mapping.from();
			mappings_.insert({std::string(from), std::move(mapping)});
		}
	}

	void traverse() const
	{
		auto mapping = mappings_.find("seed");
		auto data = seeds_;
		dump("seed", data);

		std::string to;
		do
		{
			to = mapping->second.to();
			mapping->second.transform(data);
			dump(to, data);
			if (to == "location")
			{
				break;
			}
			else
			{
				mapping = mappings_.find(to);
			}
		}
		while (to != "location");

		std::cout << std::min_element(
						 data.begin(),
						 data.end(),
						 [](const Range & lhs, const Range & rhs)
						 {
							 return lhs.start < rhs.start;
						 })
						 ->start
				  << '\n';
	}

private:
	std::vector<Range> seeds_;
	std::unordered_map<std::string, Mapping> mappings_;
};

std::vector<Range> parse_seeds(std::string_view line)
{
	line = line.substr("seeds: "sv.size());
	std::vector<unsigned> numbers;
	std::size_t end = 0;
	do
	{
		end = line.find_first_of(' ');
		numbers.push_back(atoi(line.substr(0, end).data()));
		if (end != std::string_view::npos)
		{
			line = line.substr(end + 1);
		}
		else
		{
			break;
		}
	}
	while (true);
	std::vector<Range> result;
	result.reserve(numbers.size() / 2);
	for (std::size_t i = 0; i < numbers.size() / 2; ++i)
	{
		result.push_back({numbers[2 * i], numbers[2 * i + 1]});
	}
	return result;
}

std::vector<Range> parse_seeds(std::istream & input)
{
	std::string line;
	std::vector<Range> result;
	std::getline(input, line);
	result = parse_seeds(line);
	std::getline(input, line);
	return result;
}

std::pair<std::string_view, std::string_view> parse_from_to(std::string_view str)
{
	std::size_t from_end = str.find_first_of('-');
	std::string_view from = str.substr(0, from_end);
	str = str.substr(from_end + "-to-"sv.size());
	std::string_view to = str.substr(0, str.find_first_of(' '));
	return {from, to};
}

Mapping process_mappings(std::istream & input)
{
	std::string line;
	std::getline(input, line);
	const auto [from, to] = std::pair<std::string, std::string>(parse_from_to(line));
	std::vector<MappingPart> parts;
	do
	{
		std::getline(input, line);
		if (line.empty())
		{
			break;
		}
		parts.push_back(to_mapping_part(line));
	}
	while (true);
	return {from, to, parts};
}

} // namespace

int task_5_2(std::string_view input_file)
{
	std::ifstream input(input_file.data());

	auto seeds = parse_seeds(input);
	std::vector<Mapping> mappings;
	while (!input.bad() && !input.eof())
	{
		mappings.push_back(process_mappings(input));
	}
	Almanac almanac(std::move(seeds), std::move(mappings));
	almanac.traverse();

	return 0;
}
