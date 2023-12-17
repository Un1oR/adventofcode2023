#include "h.h"

#include <algorithm>
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

using namespace std::string_view_literals;

struct Line
{
	static constexpr std::size_t capacity = 18;

	using StorageType = std::bitset<capacity>;
	using ReferenceType = StorageType::reference;
	using CellType = bool;

	static std::vector<Line> populate(std::uint8_t count, std::uint8_t size)
	{
		auto r = std::views::iota(std::uint8_t(0), count)
			| std::views::transform(
					 [size](std::uint8_t idx)
					 {
						 return Line(idx, size);
					 });
		return {r.begin(), r.end()};
	}

	Line(std::uint8_t index, std::uint8_t size)
		: index_(index)
		, size_(size)
	{
	}

	std::uint8_t index() const
	{
		return index_;
	}

	std::uint8_t size() const
	{
		return size_;
	}

	ReferenceType operator[](std::uint8_t idx)
	{
		return data_[idx];
	}

	CellType operator[](std::uint8_t idx) const
	{
		return data_[idx];
	}

	bool operator==(const Line & line) const noexcept
	{
		return data_ == line.data_;
	}

private:
	std::uint8_t index_ = std::numeric_limits<std::uint8_t>::max();
	StorageType data_ = 0;
	std::uint8_t size_ = 0;
};

void debug_print(
	std::uint8_t min_column,
	std::uint8_t max_column,
	const std::vector<Line> & rows,
	std::uint8_t min_row,
	std::uint8_t max_row)
{
	std::uint8_t column_count = max_column - min_column + 1;
	std::string_view prefix("   || ");
	std::cout << prefix;
	for (std::uint8_t j = min_column; j <= max_column; ++j)
	{
		std::cout << std::setw(2) << unsigned(j) + 1 << ' ';
	}
	std::cout << '\n' << std::string(column_count * 3 + prefix.size(), '=') << '\n';
	for (std::uint8_t i = min_row; i <= max_row; ++i)
	{
		std::cout << std::setw(2) << unsigned(i) + 1 << " || ";
		for (std::uint8_t j = min_column; j <= max_column; ++j)
		{
			std::cout << std::setw(2) << (rows[i][j] ? '#' : '.') << ' ';
		}
		std::cout << '\n';
	}
	std::cout << '\n';
}

template <bool Reversed = false>
std::int64_t process_lines(const std::vector<Line> & rows, const std::vector<Line> & columns, bool by_row, bool debug)
{
	std::span<const Line> lines = by_row ? rows : columns;
	std::uint8_t min_column = 0;
	std::uint8_t max_column = columns.size() - 1;
	std::uint8_t min_row = 0;
	std::uint8_t max_row = rows.size() - 1;
	auto it = [&]
	{
		if constexpr (Reversed)
		{
			return lines.rbegin();
		}
		else
		{
			return lines.begin();
		}
	}();
	auto end = [&]
	{
		if constexpr (Reversed)
		{
			return lines.rend();
		}
		else
		{
			return lines.end();
		}
	}();
	for (; it != end; ++it)
	{
		if (debug)
		{
			std::cout << "start idx = " << unsigned(it->index()) + 1 << '\n';
		}
		if (std::next(it) == end)
		{
			continue;
		}
		auto it1 = it, it2 = std::prev(end);
		if (std::distance(it1, it2) % 2 == 0)
		{
			continue;
		}
		bool found = true;
		for (; it1 < it2; ++it1, --it2)
		{
			if (*it1 != *it2)
			{
				found = false;
				break;
			}
		}
		if (by_row)
		{
			min_row = it->index();
			max_row = std::prev(end)->index();
		}
		else
		{
			min_column = it->index();
			max_column = std::prev(end)->index();
		}
		if (debug)
		{
			debug_print(
				std::min(min_column, max_column),
				std::max(min_column, max_column),
				rows,
				std::min(min_row, max_row),
				std::max(min_row, max_row));
		}
		if (found)
		{
			return (it->index() + 1 + std::prev(end)->index() + 1) / 2;
		}
	}
	return -1;
}

std::uint64_t process_lines(std::vector<std::string> lines)
{
	std::uint8_t row_count = lines.size();
	std::uint8_t columns_count = lines[0].size();
	std::vector<Line> rows = Line::populate(row_count, columns_count);
	std::vector<Line> columns = Line::populate(columns_count, row_count);

	for (std::size_t i = 0; i < row_count; ++i)
	{
		for (std::size_t j = 0; j < columns_count; ++j)
		{
			const bool bit = lines[i][j] == '#';
			rows[i][j] = bit;
			columns[j][i] = bit;
		}
	}

	debug_print(0, columns_count - 1, rows, 0, row_count - 1);

	const bool debug = false;
	if (debug)
	{
		std::cout << "By columns reversed <=\n";
	}
	std::int64_t by_column_reversed_result = process_lines<true>(rows, columns, false, debug);
	std::cout << "By columns reversed <=. Result = " << by_column_reversed_result << "\n\n";
	if (by_column_reversed_result > 0)
	{
		if (debug)
		{
			debug_print(0, columns_count - 1, rows, 0, row_count - 1);
		}
		return by_column_reversed_result;
	}

	if (debug)
	{
		std::cout << "By columns =>\n";
	}
	std::int64_t by_column_result = process_lines<false>(rows, columns, false, debug);
	std::cout << "By columns =>. Result = " << by_column_result << "\n\n";
	if (by_column_result > 0)
	{
		if (debug)
		{
			debug_print(0, columns_count - 1, rows, 0, row_count - 1);
		}
		return by_column_result;
	}

	if (debug)
	{
		std::cout << "By rows reversed <=\n";
	}
	std::int64_t by_rows_reversed_result = process_lines<true>(rows, columns, true, debug);
	std::cout << "By rows reversed <=. Result = " << by_rows_reversed_result << "\n\n";
	if (by_rows_reversed_result > 0)
	{
		if (debug)
		{
			debug_print(0, columns_count - 1, rows, 0, row_count - 1);
		}
		return 100 * by_rows_reversed_result;
	}

	if (debug)
	{
		std::cout << "By rows =>\n";
	}
	std::int64_t by_rows_result = process_lines<false>(rows, columns, true, debug);
	std::cout << "By rows =>. Result = " << by_rows_result << "\n\n";
	if (by_rows_result > 0)
	{
		if (debug)
		{
			debug_print(0, columns_count - 1, rows, 0, row_count - 1);
		}
		return 100 * by_rows_result;
	}

	throw std::logic_error("No symmetry");
}

} // namespace

int task_13_1(std::string_view input_file)
{
	std::ifstream input(input_file.data());

	std::uint64_t result = 0;
	std::string line;
	while (!input.bad() && !input.eof())
	{
		std::vector<std::string> lines;
		while (true)
		{
			std::getline(input, line);
			if (line.empty())
			{
				break;
			}
			lines.push_back(line);
			if (input.eof())
			{
				break;
			}
		}
		result += process_lines(std::move(lines));
	}
	std::cout << result << '\n';

	return 0;
}
