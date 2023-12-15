#include "h.h"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <fstream>
#include <functional>
#include <iostream>
#include <numeric>
#include <ranges>
#include <vector>

namespace
{

using namespace std::string_view_literals;

enum class Type : std::uint8_t
{
	Ground, // .
	Vertical, // |
	Horisontal, // -
	NorthToEast, // L
	NorthToWest, // J
	SouthToWest, // 7
	SouthToEast, // F
	Animal, // S
};

const std::unordered_map<char, Type> symbol_to_type = {
	{'.', Type::Ground},
	{'|', Type::Vertical},
	{'-', Type::Horisontal},
	{'L', Type::NorthToEast},
	{'J', Type::NorthToWest},
	{'7', Type::SouthToWest},
	{'F', Type::SouthToEast},
	{'S', Type::Animal},
};

Type to_type(const char ch)
{
	return symbol_to_type.at(ch);
}

char to_symbol(Type type)
{
	for (const auto & [k, v] : symbol_to_type)
	{
		if (v == type)
		{
			return k;
		}
	}
	throw std::logic_error("Unknown type");
}

std::vector<Type> process_line(std::string_view line)
{
	std::vector<Type> result(line.size() + 2);
	result.front() = result.back() = Type::Ground;
	const auto r = line | std::ranges::views::transform(to_type);
	std::ranges::copy(r, std::next(result.begin()));
	return result;
}

using Map = std::vector<std::vector<Type>>;

class Cursor
{
public:
	explicit Cursor(const Map & map)
		: map_(map)
	{
	}

	void start()
	{
		seek_to_animal();

		const auto is_path = [](const Type & type)
		{
			return type != Type::Animal && type != Type::Ground;
		};
		if (is_path(map_[i_ - 1][j_]))
		{
			cmp_with_prev_i_ = -1;
			--i_;
			return;
		}
		if (is_path(map_[i_ + 1][j_]))
		{
			cmp_with_prev_i_ = 1;
			++i_;
			return;
		}
		if (is_path(map_[i_][j_ - 1]))
		{
			cmp_with_prev_j_ = -1;
			--j_;
			return;
		}
		if (is_path(map_[i_][j_ + 1]))
		{
			cmp_with_prev_j_ = 1;
			++j_;
			return;
		}
		throw std::logic_error("No way");
	}

	Type current() const
	{
		return map_[i_][j_];
	}

	bool next()
	{
		switch (map_[i_][j_])
		{
		case Type::Vertical:
			i_ += cmp_with_prev_i_;
			break;

		case Type::Horisontal:
			j_ += cmp_with_prev_j_;
			break;

		case Type::NorthToEast:
			if (cmp_with_prev_i_ > 0)
			{
				++j_;
				cmp_with_prev_i_ = 0;
				cmp_with_prev_j_ = 1;
			}
			else
			{
				--i_;
				cmp_with_prev_i_ = -1;
				cmp_with_prev_j_ = 0;
			}
			break;

		case Type::NorthToWest:
			if (cmp_with_prev_i_ > 0)
			{
				--j_;
				cmp_with_prev_i_ = 0;
				cmp_with_prev_j_ = -1;
			}
			else
			{
				--i_;
				cmp_with_prev_i_ = -1;
				cmp_with_prev_j_ = 0;
			}
			break;

		case Type::SouthToWest:
			if (cmp_with_prev_i_ < 0)
			{
				--j_;
				cmp_with_prev_i_ = 0;
				cmp_with_prev_j_ = -1;
			}
			else
			{
				++i_;
				cmp_with_prev_i_ = 1;
				cmp_with_prev_j_ = 0;
			}
			break;

		case Type::SouthToEast:
			if (cmp_with_prev_i_ < 0)
			{
				++j_;
				cmp_with_prev_i_ = 0;
				cmp_with_prev_j_ = 1;
			}
			else
			{
				++i_;
				cmp_with_prev_i_ = 1;
				cmp_with_prev_j_ = 0;
			}
			break;

		case Type::Ground:
			throw std::logic_error("Path to ground");
		case Type::Animal:
			throw std::logic_error("Next without start");
		}

		return map_[i_][j_] != Type::Animal;
	}

private:
	void seek_to_animal()
	{
		for (std::size_t i = 0; i < map_.size(); ++i)
		{
			for (std::size_t j = 0; j < map_[i].size(); ++j)
			{
				if (map_[i][j] == Type::Animal)
				{
					i_ = i;
					j_ = j;
					return;
				}
			}
		}
	}

private:
	const Map & map_;
	std::size_t i_ = 0;
	int cmp_with_prev_i_ = 0;
	std::size_t j_ = 0;
	int cmp_with_prev_j_ = 0;
};

} // namespace

int task_10_1(std::string_view input_file)
{
	std::ifstream input(input_file.data());

	std::vector<std::vector<Type>> map;
	std::string line;
	std::getline(input, line);
	map.push_back(std::vector<Type>(line.size() + 2, Type::Ground));
	map.push_back(process_line(line));
	while (!input.bad() && !input.eof())
	{
		std::getline(input, line);
		map.push_back(process_line(line));
	}
	map.push_back(std::vector<Type>(line.size() + 2, Type::Ground));

	for (const auto & row : map)
	{
		for (const auto & cell : row)
		{
			std::cout << to_symbol(cell);
		}
		std::cout << '\n';
	}

	Cursor cursor(map);
	cursor.start();
	std::cout << to_symbol(cursor.current()) << '\n';
	unsigned len = 1;
	while (cursor.next())
	{
		++len;
		std::cout << to_symbol(cursor.current()) << '\n';
	}

	std::cout << "====\n";
	std::cout << std::ceil(double(len) / 2) << '\n';

	return 0;
}
