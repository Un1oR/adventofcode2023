#include "h.h"

#include <bitset>
#include <cstdint>
#include <format>
#include <fstream>
#include <iostream>
#include <ranges>
#include <stack>
#include <unordered_set>
#include <vector>

namespace
{

using namespace std::string_literals;
using namespace std::string_view_literals;

enum class TileType : std::uint8_t
{
	Empty,
	MirrorSlash,
	MirrorBackslash,
	SplitterPipe,
	SplitterDash,
};

struct TileCoordinate
{
	std::size_t i = 0;
	std::size_t j = 0;
};

enum class InputDirection : std::uint8_t
{
	Left,
	Top,
	Right,
	Bottom,
};

class Map;
class TileWithInputDirection;

class Tile
{
public:
	Tile(const Map & map, std::size_t idx, TileType type)
		: map_(map)
		, idx_(idx)
		, type_(type)
	{
	}

	std::size_t idx() const
	{
		return idx_;
	}

	TileType type() const
	{
		return type_;
	}

	std::size_t max_idx() const;

	std::vector<TileWithInputDirection> pass(InputDirection input_direction) const;

private:
	TileCoordinate coordinate() const;

	std::vector<TileWithInputDirection> pass_as_empty(InputDirection input_direction) const;
	std::vector<TileWithInputDirection> pass_as_mirror_slash(InputDirection input_direction) const;
	std::vector<TileWithInputDirection> pass_as_mirror_backslash(InputDirection input_direction) const;
	std::vector<TileWithInputDirection> pass_as_splitter_pipe(InputDirection input_direction) const;
	std::vector<TileWithInputDirection> pass_as_splitter_dash(InputDirection input_direction) const;

private:
	const Map & map_;
	const std::size_t idx_;
	const TileType type_;
};

class TileWithInputDirection
{
public:
	TileWithInputDirection(Tile tile, InputDirection input_direction)
		: tile_(tile)
		, input_direction_(input_direction)
	{
	}

	std::vector<TileWithInputDirection> next() const
	{
		return tile_.pass(input_direction_);
	}

	std::size_t idx() const
	{
		return tile_.idx();
	}

	std::size_t rank() const
	{
		return tile_.max_idx() * static_cast<std::size_t>(input_direction_) + idx();
	}

	bool operator==(const TileWithInputDirection & other) const noexcept
	{
		return rank() == other.rank();
	}

private:
	Tile tile_;
	InputDirection input_direction_;
};

} // namespace

template <>
struct std::hash<TileWithInputDirection>
{
	std::size_t operator()(const TileWithInputDirection & tile_with_input_direction) const noexcept
	{
		return std::hash<std::size_t>()(tile_with_input_direction.rank());
	}
};

namespace
{

class Map
{
public:
	explicit Map(std::string_view first_line)
		: line_len_(first_line.size())
	{
		append_line(first_line);
	}

	void append_line(std::string_view line)
	{
		const auto start_index = line_count_ * line_len_;
		auto r = line
			| std::views::transform(
					 [idx = start_index, &map = *this](const char ch) mutable
					 {
						 const auto type = [&]
						 {
							 if (ch == '.')
							 {
								 return TileType::Empty;
							 }
							 if (ch == '/')
							 {
								 return TileType::MirrorSlash;
							 }
							 if (ch == '\\')
							 {
								 return TileType::MirrorBackslash;
							 }
							 if (ch == '|')
							 {
								 return TileType::SplitterPipe;
							 }
							 if (ch == '-')
							 {
								 return TileType::SplitterDash;
							 }
							 throw std::logic_error("Unknown tile type");
						 }();
						 return Tile(map, idx++, type);
					 });
		std::ranges::copy(r, std::back_insert_iterator(tiles_));
		++line_count_;
	}

	TileWithInputDirection start() const
	{
		return TileWithInputDirection(get(TileCoordinate(0, 0)), InputDirection::Left);
	}

	void print_passed(std::unordered_set<std::size_t> passed)
	{
		for (std::size_t i = 0; i < line_len_; ++i)
		{
			for (std::size_t j = 0; j < line_count_; ++j)
			{
				if (passed.contains(encode_idx(i, j)))
				{
					std::cout << '#';
				}
				else
				{
					std::cout << '.';
				}
			}
			std::cout << '\n';
		}
		std::cout << '\n';
	}

private:
	Tile get(TileCoordinate tile_coordinate) const
	{
		return tiles_[tile_coordinate.i * line_len_ + tile_coordinate.j];
	}

	TileCoordinate decode_idx(std::size_t idx) const
	{
		return {idx / line_len_, idx % line_len_};
	}

	std::size_t encode_idx(std::size_t i, std::size_t j) const
	{
		return i * line_len_ + j;
	}

	std::optional<TileCoordinate> shift(TileCoordinate tile_coordinate, InputDirection input_direction) const
	{
		switch (input_direction)
		{
		case InputDirection::Left:
			if (tile_coordinate.j == line_len_ - 1)
			{
				return std::nullopt;
			}
			++tile_coordinate.j;
			return tile_coordinate;

		case InputDirection::Top:
			if (tile_coordinate.i == line_count_ - 1)
			{
				return std::nullopt;
			}
			++tile_coordinate.i;
			return tile_coordinate;

		case InputDirection::Right:
			if (tile_coordinate.j == 0)
			{
				return std::nullopt;
			}
			--tile_coordinate.j;
			return tile_coordinate;

		case InputDirection::Bottom:
			if (tile_coordinate.i == 0)
			{
				return std::nullopt;
			}
			--tile_coordinate.i;
			return tile_coordinate;
		}
	}

	friend class Tile;

private:
	const std::size_t line_len_ = 0;
	std::size_t line_count_ = 0;
	std::vector<Tile> tiles_;
};

std::size_t Tile::max_idx() const
{
	return map_.line_len_ * map_.line_count_;
}

std::vector<TileWithInputDirection> Tile::pass(InputDirection input_direction) const
{
	switch (type_)
	{
	case TileType::Empty:
		return pass_as_empty(input_direction);

	case TileType::MirrorSlash:
		return pass_as_mirror_slash(input_direction);

	case TileType::MirrorBackslash:
		return pass_as_mirror_backslash(input_direction);

	case TileType::SplitterPipe:
		return pass_as_splitter_pipe(input_direction);

	case TileType::SplitterDash:
		return pass_as_splitter_dash(input_direction);
	}
}

TileCoordinate Tile::coordinate() const
{
	return map_.decode_idx(idx_);
}

std::vector<TileWithInputDirection> Tile::pass_as_empty(InputDirection input_direction) const
{
	if (const auto result_coordinate = map_.shift(coordinate(), input_direction))
	{
		return {{map_.get(*result_coordinate), input_direction}};
	}
	return {};
}

std::vector<TileWithInputDirection> Tile::pass_as_mirror_slash(InputDirection input_direction) const
{
	const auto result_input_direction = [input_direction]
	{
		switch (input_direction)
		{
		case InputDirection::Left:
			return InputDirection::Bottom;

		case InputDirection::Top:
			return InputDirection::Right;

		case InputDirection::Right:
			return InputDirection::Top;

		case InputDirection::Bottom:
			return InputDirection::Left;
		}
	}();
	if (const auto result_coordinate = map_.shift(coordinate(), result_input_direction))
	{
		return {{map_.get(*result_coordinate), result_input_direction}};
	}
	return {};
}

std::vector<TileWithInputDirection> Tile::pass_as_mirror_backslash(InputDirection input_direction) const
{
	const auto result_input_direction = [input_direction]
	{
		switch (input_direction)
		{
		case InputDirection::Left:
			return InputDirection::Top;

		case InputDirection::Top:
			return InputDirection::Left;

		case InputDirection::Right:
			return InputDirection::Bottom;

		case InputDirection::Bottom:
			return InputDirection::Right;
		}
	}();
	if (const auto result_coordinate = map_.shift(coordinate(), result_input_direction))
	{
		return {{map_.get(*result_coordinate), result_input_direction}};
	}
	return {};
}

std::vector<TileWithInputDirection> Tile::pass_as_splitter_pipe(InputDirection input_direction) const
{
	switch (input_direction)
	{
	case InputDirection::Top:
	case InputDirection::Bottom:
		return pass_as_empty(input_direction);

	case InputDirection::Left:
	case InputDirection::Right:
		{
			const auto current_coordinate = coordinate();
			std::vector<TileWithInputDirection> result;
			if (const auto result_coordinate = map_.shift(current_coordinate, InputDirection::Top))
			{
				result.emplace_back(map_.get(*result_coordinate), InputDirection::Top);
			}
			if (const auto result_coordinate = map_.shift(current_coordinate, InputDirection::Bottom))
			{
				result.emplace_back(map_.get(*result_coordinate), InputDirection::Bottom);
			}
			return result;
		}
	}
}

std::vector<TileWithInputDirection> Tile::pass_as_splitter_dash(InputDirection input_direction) const
{
	switch (input_direction)
	{
	case InputDirection::Left:
	case InputDirection::Right:
		return pass_as_empty(input_direction);

	case InputDirection::Top:
	case InputDirection::Bottom:
		{
			const auto current_coordinate = coordinate();
			std::vector<TileWithInputDirection> result;
			if (const auto result_coordinate = map_.shift(current_coordinate, InputDirection::Left))
			{
				result.emplace_back(map_.get(*result_coordinate), InputDirection::Left);
			}
			if (const auto result_coordinate = map_.shift(current_coordinate, InputDirection::Right))
			{
				result.emplace_back(map_.get(*result_coordinate), InputDirection::Right);
			}
			return result;
		}
	}
}

} // namespace

int task_16_1(std::string_view input_file)
{
	std::ifstream input(input_file.data());

	std::string line;
	std::getline(input, line);
	Map map(line);
	while (std::getline(input, line))
	{
		map.append_line(line);
	}

	std::stack<TileWithInputDirection> stack;
	stack.push(map.start());
	std::unordered_set<TileWithInputDirection> cache;
	std::unordered_set<std::size_t> passed;
	while (!stack.empty())
	{
//		map.print_passed(passed);
//		std::cout << '\n';
		const auto current = stack.top();
//		std::cout << current.idx() << '\n';
		cache.insert(current);
		passed.insert(current.idx());
		stack.pop();
		for (const auto & tile_with_direction : current.next())
		{
			if (!cache.contains(tile_with_direction))
			{
				stack.push(tile_with_direction);
			}
		}
	}

	std::cout << "====\n";
	std::cout << passed.size() << '\n';

	return 0;
}
