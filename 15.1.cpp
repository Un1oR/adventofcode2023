#include "h.h"

#include <bitset>
#include <cstdint>
#include <format>
#include <fstream>
#include <iostream>

namespace
{

using namespace std::string_literals;
using namespace std::string_view_literals;

} // namespace

int task_15_1(std::string_view input_file)
{
	std::ifstream input(input_file.data());

	std::uint64_t result = 0;
	std::string word;
	while (std::getline(input, word, ','))
	{
		std::uint64_t word_result = 0;
		for (const auto ch : word)
		{
			word_result += ch;
			word_result *= 17;
			word_result %= 256;
		}
		result += word_result;
	}
	std::cout << result << '\n';

	return 0;
}
