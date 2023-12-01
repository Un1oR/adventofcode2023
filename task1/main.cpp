#include <iostream>
#include <fstream>
#include <string>
#include <string_view>


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


int main(int argc, char ** argv)
{
	if (argc < 2)
	{
		return 1;
	}
	std::ifstream input(argv[1]);
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

