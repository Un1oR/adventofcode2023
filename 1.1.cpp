#include "1.1.h"

#include <fstream>
#include <iostream>

namespace
{

int extract_number(std::string_view str)
{
	int first = -1;
	int second = -1;
	std::size_t idx = 0;
	for (; idx < str.size(); ++idx)
	{
		auto ch = str[idx];
		if ('0' <= ch && ch <= '9')
		{
			first = ch - '0';
			break;
		}
	}
	for (; idx < str.size(); ++idx)
	{
		auto ch = str[idx];
		if ('0' <= ch && ch <= '9')
		{
			second = ch - '0';
		}
	}
	if (first < 0 || second < 0)
	{
		return 0;
	}
	return first * 10 + second;
}

} // namespace

int task_1_1(std::string_view input_file)
{
	std::ifstream input(input_file.data());
	std::string line;
	int result = 0;
	while (!input.bad() && !input.eof())
	{
		std::getline(input, line);
		result += extract_number(line);
	}
	std::cout << result << '\n';
	return 0;
}
