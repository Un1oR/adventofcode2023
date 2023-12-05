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

	unsigned map(unsigned src) const
	{
		const auto it = std::lower_bound(
			parts_.rbegin(),
			parts_.rend(),
			src,
			[](const MappingPart lhs, const unsigned & rhs)
			{
				return lhs.src_start > rhs;
			});
		if (it == parts_.rend())
		{
			return src;
		}
		if ((src < it->src_start) || (src > (it->src_start + it->len)))
		{
			return src;
		}
		return src - it->src_start + it->dst_start;
	}

	void transform(std::vector<unsigned> & data) const
	{
		for (auto & item : data)
		{
			auto src = item;
			item = map(src);
		}
	}

private:
	std::string from_;
	std::string to_;
	std::vector<MappingPart> parts_;
};

void dump(std::string_view type, const std::vector<unsigned> & values)
{
	std::cout << type << ": ";
	for (const unsigned value : values)
	{
		std::cout << value << " ";
	}
	std::cout << '\n';
}

class Almanac
{
public:
	Almanac(std::vector<unsigned> seeds, std::vector<Mapping> mappings)
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

		std::cout << *std::min_element(data.begin(), data.end()) << '\n';
	}

private:
	std::vector<unsigned> seeds_;
	std::unordered_map<std::string, Mapping> mappings_;
};

std::vector<unsigned> parse_seeds(std::string_view line)
{
	line = line.substr("seeds: "sv.size());
	std::vector<unsigned> result;
	std::size_t end = 0;
	do
	{
		end = line.find_first_of(' ');
		result.push_back(atoi(line.substr(0, end).data()));
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
	return result;
}

std::vector<unsigned> parse_seeds(std::istream & input)
{
	std::string line;
	std::vector<unsigned> result;
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

int task_5_1(std::string_view input_file)
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
