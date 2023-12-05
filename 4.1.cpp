#include "h.h"

#include <fstream>
#include <functional>
#include <iostream>
#include <ranges>
#include <unordered_set>

namespace
{

void on_numbers(std::string_view line, [[maybe_unused]] const std::function<void(unsigned)> & f)
{
	for (std::size_t i = 0; i < line.size() / 3; ++i)
	{
		unsigned number = 0;
		if (line[3*i + 1] != ' ')
		{
			number += 10 * (line[3*i + 1] - '0');
		}
		number += (line[3*i + 2] - '0');
		f(number);
	}
}

unsigned process_line(std::string_view line)
{
	line = line.substr(line.find_first_of(':') + 1);
	std::size_t pipe_pos = line.find_first_of('|');
	std::string_view win = line.substr(0, pipe_pos - 1);
	std::string_view my = line.substr(pipe_pos + 1);

	std::unordered_set<unsigned> win_numbers;
	on_numbers(
		win,
		[&win_numbers](unsigned number)
		{
			win_numbers.insert(number);
		});

	unsigned result = 0;
	on_numbers(
		my,
		[&win_numbers, &result](unsigned number)
		{
			if (win_numbers.contains(number))
			{
				if (!result)
				{
					result = 1;
				}
				else
				{
					result *= 2;
				}
			}
		});
	return result;
}

} // namespace

int task_4_1(std::string_view input_file)
{
	std::ifstream input(input_file.data());

	std::string line;
	unsigned result = 0;
	while (!input.bad() && !input.eof())
	{
		std::getline(input, line);
		result += process_line(line);
	}
	std::cout << result << '\n';

	return 0;
}
