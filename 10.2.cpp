#include "h.h"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <fstream>
#include <functional>
#include <iostream>
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

struct Coordinate
{
	std::size_t i = 0;
	std::size_t j = 0;

	constexpr bool operator==(const Coordinate &) const = default;
};

struct Point
{
	Coordinate coordinate;
	Type type;

	constexpr bool operator==(const Point & other) const
	{
		return coordinate == other.coordinate;
	}
};

class Cursor
{
public:
	explicit Cursor(const Map & map)
		: map_(map)
	{
	}

	Point start()
	{
		seek_to_animal();
		Point result = current();

		if (
			[](Type type)
			{
				return type == Type::SouthToEast || type == Type::Vertical || type == Type::SouthToWest;
			}(map_[i_ - 1][j_]))
		{
			cmp_with_prev_i_ = -1;
			--i_;
		}
		else if (
			[](Type type)
			{
				return type == Type::NorthToWest || type == Type::Vertical || type == Type::NorthToEast;
			}(map_[i_ + 1][j_]))
		{
			cmp_with_prev_i_ = 1;
			++i_;
		}
		else if (
			[](Type type)
			{
				return type == Type::NorthToEast || type == Type::Horisontal || type == Type::SouthToEast;
			}(map_[i_][j_ - 1]))
		{
			cmp_with_prev_j_ = -1;
			--j_;
		}
		else if (
			[](Type type)
			{
				return type == Type::NorthToWest || type == Type::Horisontal || type == Type::SouthToWest;
			}(map_[i_][j_ + 1]))
		{
			cmp_with_prev_j_ = 1;
			++j_;
		}
		else
		{
			throw std::logic_error("No way");
		}
		return result;
	}

	Point current() const
	{
		return {{i_, j_}, map_[i_][j_]};
	}

	std::optional<Point> next()
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

		return map_[i_][j_] != Type::Animal ? std::optional(Point{{i_, j_}, map_[i_][j_]}) : std::nullopt;
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

template <>
struct std::hash<Coordinate>
{
	std::size_t operator()(const Coordinate & point) const noexcept
	{
		std::size_t h1 = std::hash<std::size_t>{}(point.i);
		std::size_t h2 = std::hash<std::size_t>{}(point.j);
		return h1 | (h2 << sizeof(std::size_t) * 4);
	}
};

namespace
{

class Contour
{
public:
	void push_back(Point point)
	{
		points_.push_back(point);
		index_[point.coordinate] = points_.size() - 1;
	}

	std::optional<Type> get_type(std::size_t i, std::size_t j) const
	{
		auto it = index_.find(Coordinate{i, j});
		return it == index_.cend() //
			? std::nullopt
			: std::optional(points_[it->second].type);
	}

	std::optional<int> is_intersection(std::size_t i, std::size_t j) const
	{
		if (auto type = get_type(i, j))
		{
			if (*type == Type::Animal)
			{
				type = to_real_type(i, j);
			}
			switch (*type)
			{
			case Type::Vertical:
				return 0;

			case Type::NorthToEast:
			case Type::NorthToWest:
				return -1;

			case Type::SouthToWest:
			case Type::SouthToEast:
				return 1;

			case Type::Horisontal:
				return std::nullopt;

			case Type::Ground:
			case Type::Animal:
				throw std::logic_error("Invalid type on contour");
			}
		}
		return std::nullopt;
	};

	Type to_real_type(std::size_t i, std::size_t j) const
	{
		if (auto it = index_.find(Coordinate{i, j}); it->second != 0 || points_[it->second].type != Type::Animal)
		{
			throw std::logic_error("First point is not animal");
		}
		std::vector<Coordinate> neighbours{points_[points_.size() - 1].coordinate, points_[1].coordinate};
		int max_i = std::max(neighbours[0].i, neighbours[1].i);
		int max_j = std::max(neighbours[0].j, neighbours[1].j);
		int min_i = std::min(neighbours[0].i, neighbours[1].i);
		int min_j = std::min(neighbours[0].j, neighbours[1].j);
		auto diff_i_1 = min_i - int(i);
		auto diff_i_2 = max_i - int(i);
		auto diff_j_1 = min_j - int(j);
		auto diff_j_2 = max_j - int(j);
		auto diff_i = diff_i_1 + diff_i_2;
		auto diff_j = diff_j_1 + diff_j_2;
		if (diff_i > 0)
		{
			if (diff_j > 0)
			{
				return Type::SouthToEast; // F
			}
			return Type::SouthToWest; // 7
		}
		if (diff_j > 0)
		{
			return Type::NorthToEast; // L
		}
		return Type::NorthToWest; // J
	}

	std::size_t size() const
	{
		return points_.size();
	}

private:
	std::unordered_map<Coordinate, std::size_t> index_;
	std::vector<Point> points_;
};

} // namespace

int task_10_2(std::string_view input_file)
{
	std::ifstream input(input_file.data());

	std::vector<std::vector<Type>> map;
	std::string line;
	std::getline(input, line);
	std::size_t real_line_len = line.size();
	map.emplace_back(real_line_len + 2, Type::Ground);
	map.push_back(process_line(line));
	while (!input.bad() && !input.eof())
	{
		std::getline(input, line);
		map.push_back(process_line(line));
	}
	map.emplace_back(line.size() + 2, Type::Ground);

	for (const auto & row : map)
	{
		for (const auto & cell : row)
		{
			std::cout << to_symbol(cell);
		}
		std::cout << '\n';
	}

	Contour contour;
	Cursor cursor(map);
	contour.push_back(cursor.start());
	contour.push_back(cursor.current());
	std::cout << to_symbol(cursor.current().type) << '\n';
	while (auto point = cursor.next())
	{
		contour.push_back(*point);
		std::cout << to_symbol(cursor.current().type) << '\n';
	}
	std::cout << "====\n";

	unsigned result = 0;
	for (std::size_t i = 1; i <= map.size() - 2; ++i)
	{
		for (std::size_t j = 1; j <= real_line_len + 1; ++j)
		{
			if (const auto type = contour.get_type(i, j))
			{
				std::cout << to_symbol(*type);
				continue;
			}
			unsigned intersections_count = 0;
			std::optional<int> prev_intersection;
			for (std::size_t jj = j + 1; jj <= real_line_len + 1; ++jj)
			{
				const auto intesection = contour.is_intersection(i, jj);
				if (!intesection)
				{
					continue;
				}
				if (*intesection == 0)
				{
					intersections_count += 1;
					prev_intersection.reset();
					continue;
				}
				if (!prev_intersection)
				{
					prev_intersection = intesection;
					continue;
				}
				if (prev_intersection == intesection)
				{
					intersections_count += 2;
				}
				else
				{
					intersections_count += 1;
				}
				prev_intersection.reset();
			}
			unsigned not_even = intersections_count % 2;
			std::cout << (not_even ? '$' : '*');
			result += not_even;
		}
		std::cout << '\n';
	}

	std::cout << result << '\n';

	return 0;
}
