#include "h.h"

#include <algorithm>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <numeric>
#include <span>
#include <unordered_map>
#include <vector>

namespace
{

using namespace std::string_view_literals;

std::vector<bool> parse_instructions(std::string_view line)
{
	std::vector<bool> result;
	while (!line.empty())
	{
		result.push_back(line[0] == 'R');
		line = line.substr(1);
	}
	return result;
}

using Node = std::pair<std::string, std::pair<std::string, std::string>>;

Node parse_node(std::string_view line)
{
	Node result;
	std::size_t e = 0;
	e = line.find_first_of(' ');
	result.first = std::string(line.substr(0, e));
	line = line.substr(line.find_first_of('(') + 1);
	e = line.find_first_of(',');
	result.second.first = line.substr(0, e);
	line = line.substr(line.find_first_of(' ') + 1);
	e = line.find_first_of(')');
	result.second.second = std::string(line.substr(0, e));
	return result;
}

uint64_t get_single_path_len(
	const std::unordered_map<std::string, std::pair<std::string, std::string>> & map,
	std::string start_point,
	std::vector<bool> instructions)
{
	auto next_instruction = [idx = std::size_t(0), instructions = std::move(instructions)]() mutable -> bool
	{
		auto rv = instructions[idx];
		if (++idx == instructions.size())
		{
			idx = 0;
		}
		return rv;
	};

	auto node_it = map.find(start_point);
	if (node_it == map.end())
	{
		throw std::logic_error("No " + start_point + " node");
	}
	unsigned len = 0;
	while (!node_it->first.ends_with("Z"))
	{
		++len;
		const auto & next = next_instruction() ? node_it->second.second : node_it->second.first;
		std::cout << "next = " << next << '\n';
		node_it = map.find(next);
	}
	return len;
}

std::uint64_t get_path_len(
	std::unordered_map<std::string, std::pair<std::string, std::string>> map,
	std::vector<bool> instructions)
{
	std::uint64_t result = 1;
	for (const auto & [k, v] : map)
	{
		if (k.ends_with("A"))
		{
			std::cout << k << ":\n";
			auto len = get_single_path_len(map, k, instructions);
			std::cout << "len = " << len << "\n";
			std::cout << "===\n";
			result = std::lcm<std::uint64_t>(result, len);
		}
	}
	return result;
}

} // namespace

int task_8_2(std::string_view input_file)
{
	std::ifstream input(input_file.data());

	std::string line;
	std::getline(input, line);
	const auto instructions = parse_instructions(line);
	std::getline(input, line);

	std::unordered_map<std::string, std::pair<std::string, std::string>> map;
	while (!input.bad() && !input.eof())
	{
		std::getline(input, line);
		map.insert(parse_node(line));
	}
	std::uint64_t result = get_path_len(std::move(map), std::move(instructions));
	std::cout << result << '\n';

	return 0;
}
