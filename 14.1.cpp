#include "h.h"

#include <algorithm>
#include <bitset>
#include <cstdint>
#include <format>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <ranges>
#include <vector>

namespace
{

using namespace std::string_literals;
using namespace std::string_view_literals;

class Processor
{
public:
	Processor(std::int64_t current_line, std::size_t line_size)
		: correction_(line_size, 0)
		, current_line_(current_line)
	{
	}

	std::int64_t process_line(std::string_view line)
	{
		std::cout << std::setw(3) << current_line_ << ": ";
		for (const char ch : line)
		{
			std::cout << "  " << ch << " ";
		}
		std::cout << "\n     ";
		std::int64_t result = 0;
		for (std::size_t i = 0; i < line.size(); ++i)
		{
			const auto ch = line[i];
			if (ch == '.')
			{
				if (!correction_[i])
				{
					correction_[i] = current_line_;
				}
				std::cout << "    ";
			}
			else if (ch == '#')
			{
				correction_[i] = 0;
				std::cout << "    ";
			}
			else // ch == 'O'
			{
				std::int64_t sum = 0;
				if (correction_[i])
				{
					sum = correction_[i];
					result += sum;
					--correction_[i];
				}
				else
				{
					sum = current_line_;
					result += sum;
				}
				std::cout << std::setw(3) << sum << " ";
			}
		}
		std::cout << "\n\n";
		--current_line_;
		return result;
	}

private:
	std::vector<std::int64_t> correction_;
	std::int64_t current_line_;
};

} // namespace

int task_14_1(std::string_view input_file)
{
	std::ifstream input(input_file.data());
	input.seekg(0, std::ios::end);
	std::size_t file_size = input.tellg();
	input.seekg(0, std::ios::beg);

	std::uint64_t result = 0;
	std::string line;

	std::getline(input, line);
	std::size_t line_count = file_size / line.size();
	Processor processor(line_count, line.size());
	result += processor.process_line(line);

	while (!input.bad() && !input.eof())
	{
		std::getline(input, line);
		result += processor.process_line(line);
	}
	std::cout << result << '\n';

	return 0;
}
