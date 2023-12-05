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
