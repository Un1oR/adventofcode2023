#include "h.h"

#include <string_view>
#include <unordered_map>

int main(int argc, char ** argv)
{
	if (argc != 3)
	{
		return 1;
	}

	std::string_view task(argv[1]);
	std::string_view input(argv[2]);

	using namespace std::string_view_literals;
	std::unordered_map<std::string_view, int (*)(std::string_view)> tasks{
		{"1.1", &task_1_1},
		{"1.2", &task_1_2},
		{"2.1", &task_2_1},
		{"2.2", &task_2_2},
		{"3.1", &task_3_1},
		{"3.2", &task_3_2},
		{"4.1", &task_4_1},
		{"4.2", &task_4_2},
		{"5.1", &task_5_1},
		{"5.2", &task_5_2},
		{"6.1", &task_6_1},
		{"6.2", &task_6_2},
		{"7.1", &task_7_1},
		{"7.2", &task_7_2},
		{"8.1", &task_8_1},
		{"8.2", &task_8_2},
		{"9.1", &task_9_1},
		{"9.2", &task_9_2},
		{"10.1", &task_10_1},
		{"10.2", &task_10_2},
		{"11.1", &task_11_1},
		{"11.2", &task_11_2},
		{"12.1", &task_12_1},
		{"12.2", &task_12_2},
		{"13.1", &task_13_1},
	};
	if (auto it = tasks.find(task); it == tasks.end())
	{
		return 1;
	}
	else
	{
		return it->second(input);
	}
}
