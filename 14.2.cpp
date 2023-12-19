#include "h.h"

#include <algorithm>
#include <bitset>
#include <cstdint>
#include <format>
#include <functional>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <ranges>
#include <vector>

namespace
{

using namespace std::string_literals;
using namespace std::string_view_literals;

std::size_t to_position(std::size_t width, std::size_t i, std::size_t j)
{
	return i * width + j;
}

class Processor
{
public:
	Processor(std::size_t width, std::size_t height, std::vector<bool> data, std::vector<bool> map)
		: width_(width)
		, height_(height)
		, data_(std::move(data))
		, map_(std::move(map))
	{
	}

	std::uint64_t get_result() const
	{
		std::uint64_t result = 0;
		std::size_t current_line = height_;
		const auto position = std::bind_front(to_position, width_);
		for (std::size_t i = 0; i < height_; ++i)
		{
			std::cout << std::setw(3) << current_line << ": ";
			for (std::size_t j = 0; j < width_; ++j)
			{
				auto pos = position(i, j);
				std::cout << "   " << (map_[pos] ? '#' : (data_[pos] ? 'O' : '.'));
			}
			std::cout << "\n     ";
			for (std::size_t j = 0; j < width_; ++j)
			{
				auto pos = position(i, j);
				if (!map_[pos] && data_[pos])
				{
					std::cout << ' ' << std::setw(3) << current_line;
					result += current_line;
				}
				else
				{
					std::cout << "    ";
				}
			}
			std::cout << '\n';
			--current_line;
		}
		return result;
	}

private:
	const std::size_t width_;
	const std::size_t height_;
	std::vector<bool> data_;
	std::vector<bool> map_;
};

Processor read_data(std::ifstream & input)
{
	input.seekg(0, std::ios::end);
	std::size_t file_size = input.tellg();
	input.seekg(0, std::ios::beg);

	std::string line;
	std::getline(input, line);

	std::size_t width = line.size();
	std::size_t height = file_size / width;
	std::vector<bool> data(width * height, false);
	std::vector<bool> map(width * height, false);

	const auto process_line = [&data, &map, width](std::string_view line, std::size_t i)
	{
		std::size_t position = to_position(width, i, 0);
		for (std::size_t j = 0; j < line.size(); ++j, ++position)
		{
			if (line[j] == '#')
			{
				map[position] = true;
			}
			else
			{
				data[position] = line[j] == 'O';
			}
		}
	};

	process_line(line, 0);
	std::size_t i = 1;
	while (!input.bad() && !input.eof())
	{
		std::getline(input, line);
		process_line(line, i++);
	}
	return Processor(width, height, std::move(data), std::move(map));
}

} // namespace

int task_14_2(std::string_view input_file)
{
	std::ifstream input(input_file.data());
	Processor processor = read_data(input);
	auto result = processor.get_result();
	std::cout << result << '\n';

	return 0;
}
