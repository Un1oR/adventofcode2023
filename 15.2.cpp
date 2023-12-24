#include "h.h"

#include <bitset>
#include <cstdint>
#include <deque>
#include <format>
#include <fstream>
#include <iostream>
#include <ranges>

namespace
{

using namespace std::string_literals;
using namespace std::string_view_literals;

} // namespace

std::size_t hash(std::string_view word)
{
	std::size_t word_result = 0;
	for (const auto ch : word)
	{
		word_result += ch;
		word_result *= 17;
		word_result %= 256;
	}
	std::cout << "word " << word << " -> " << word_result << '\n';
	return word_result;
}

struct Lens
{
	std::string name;
	unsigned power;
};

int task_15_2(std::string_view input_file)
{
	std::ifstream input(input_file.data());

	std::string word;
	std::array<std::deque<Lens>, 256> boxes;
	while (std::getline(input, word, ','))
	{
		auto word_sv = std::string_view(word);
		std::size_t delim_idx = word_sv.find_first_of("=-");
		std::string_view name = word_sv.substr(0, delim_idx);
		std::size_t box_idx = hash(name);
		auto & box = boxes[box_idx];
		if (word_sv[delim_idx] == '-')
		{
			std::erase_if(
				box,
				[name](const Lens & lens)
				{
					return lens.name == name;
				});
		}
		else
		{
			unsigned power = std::atoi(word_sv.substr(delim_idx + 1).data());
			auto it = std::ranges::find_if(
				box,
				[name](const Lens & lens)
				{
					return lens.name == name;
				});
			if (it == box.end())
			{
				box.emplace_back(std::string(name), power);
			}
			else
			{
				it->power = power;
			}
		}
	}

	std::uint64_t result = 0;
	for (std::size_t i = 0; i < boxes.size(); ++i)
	{
		for (std::size_t j = 0; j < boxes[i].size(); ++j)
		{
			result += (i + 1) * (j + 1) * boxes[i][j].power;
		}
	}
	std::cout << result << '\n';

	return 0;
}
