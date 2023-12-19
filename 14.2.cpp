#include "h.h"

#include <bitset>
#include <cstdint>
#include <format>
#include <fstream>
#include <functional>
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

	std::uint64_t get_result(bool debug = false) const
	{
		std::uint64_t result = 0;
		std::size_t current_line = height_;
		const auto position = std::bind_front(to_position, width_);
		for (std::size_t i = 0; i < height_; ++i)
		{
			if (debug)
			{
				std::cout << std::setw(3) << current_line << ": ";
				for (std::size_t j = 0; j < width_; ++j)
				{
					auto pos = position(i, j);
					std::cout << "   " << (map_[pos] ? '#' : (data_[pos] ? 'O' : '.'));
				}
				std::cout << "\n     ";
			}
			for (std::size_t j = 0; j < width_; ++j)
			{
				auto pos = position(i, j);
				if (!map_[pos] && data_[pos])
				{
					if (debug)
					{
						std::cout << ' ' << std::setw(3) << current_line;
					}
					result += current_line;
				}
				else
				{
					if (debug)
					{
						std::cout << "    ";
					}
				}
			}
			if (debug)
			{
				std::cout << '\n';
			}
			--current_line;
		}
		return result;
	}

	void spin(std::size_t count)
	{
		std::unordered_map<std::vector<bool>, std::size_t> cache;
		std::size_t i = 0;
		for (; i < count; ++i)
		{
			spin();
			std::cout << std::hash<std::vector<bool>>()(data_) << ' ' << get_result() << '\n';
			auto it = cache.find(data_);
			if (it != cache.end())
			{
				break;
			}
			cache[data_] = i;
		}
		std::cout << "i - cache[data_] = " << i - cache[data_] << '\n';
		std::size_t loop_len = i - cache[data_];
		std::size_t tail = (count - i - 1) % loop_len;
		std::cout << "tail = " << tail << '\n';
		for (const auto & [data, iter] : cache)
		{
			if (iter == i - (loop_len - tail))
			{
				data_ = data;
				break;
			}
		}
		std::cout << std::hash<std::vector<bool>>()(data_) << ' ' << get_result() << '\n';
	}

	void spin()
	{
		rotate_to_north();
		rotate_to_west();
		rotate_to_south();
		rotate_to_east();
	}

private:
	void rotate_to_north()
	{
		const auto position = [width = width_](std::size_t i, std::size_t j)
		{
			return i * width + j;
		};
		rotate(width_, height_, position, width_);
	}

	void rotate_to_west()
	{
		const auto position = [width = width_](std::size_t j, std::size_t i)
		{
			return i * width + j;
		};
		rotate(height_, width_, position, 1);
	}

	void rotate_to_south()
	{
		const auto position = [width = width_, height = height_](std::size_t i, std::size_t j)
		{
			return (height - i - 1) * width + j;
		};
		rotate(width_, height_, position, -width_);
	}

	void rotate_to_east()
	{
		const auto position = [width = width_](std::size_t j, std::size_t i)
		{
			return i * width + (width - j - 1);
		};
		rotate(height_, width_, position, -1);
	}

	void rotate(
		const std::size_t width,
		const std::size_t height,
		std::function<std::size_t(std::size_t, std::size_t)> position,
		int free_diff)
	{
		std::vector<std::size_t> free(width, std::string::npos);
		for (std::size_t i = 0; i < width; ++i)
		{
			for (std::size_t j = 0; j < height; ++j)
			{
				const auto pos = position(i, j);
				if (map_[pos])
				{
					free[j] = std::string::npos;
				}
				else
				{
					if (!data_[pos])
					{
						if (free[j] == std::string::npos)
						{
							free[j] = pos;
						}
					}
					else
					{
						if (free[j] != std::string::npos)
						{
							data_[free[j]] = true;
							data_[pos] = false;
							free[j] += free_diff;
						}
					}
				}
			}
		}
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
	processor.spin(1'000'000'000);
	auto result = processor.get_result();
	std::cout << result << '\n';

	return 0;
}
