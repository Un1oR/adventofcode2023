#include "h.h"

#include <fstream>
#include <iostream>

namespace
{

constexpr std::string_view digits[] = {
	"one",
	"two",
	"three",
	"four",
	"five",
	"six",
	"seven",
	"eight",
	"nine",
};

int check_number(std::string_view str)
{
	for (std::size_t i = 0; i < std::size(digits); ++i)
	{
		const auto digit = digits[static_cast<std::size_t>(i)];
		if (str.starts_with(digit))
		{
			return i + 1;
		}
	}
	const auto ch = str[0];
	if ('0' <= ch && ch <= '9')
	{
		return ch - '0';
	}
	return -1;
}

int extract_number_v2(std::string_view str)
{
	int first = -1;
	int second = -1;

	while (!str.empty())
	{
		auto digit = check_number(str);
		if (digit > 0)
		{
			first = digit;
			break;
		}
		str = str.substr(1);
	}
	while (!str.empty())
	{
		auto digit = check_number(str);
		if (digit > 0)
		{
			second = digit;
		}
		str = str.substr(1);
	}
	if (first < 0 || second < 0)
	{
		return 0;
	}
	return first * 10 + second;
}

} // namespace

int task_1_2(std::string_view input_file)
{
	std::ifstream input(input_file.data());
	std::string line;
	int result = 0;
	while (!input.bad() && !input.eof())
	{
		std::getline(input, line);
		result += extract_number_v2(line);
	}
	std::cout << result << '\n';
	return 0;
}
