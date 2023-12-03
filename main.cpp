#include "h.h"

#include <string_view>

int main(int argc, char ** argv)
{
	if (argc != 3)
	{
		return 1;
	}

	std::string_view task(argv[1]);
	std::string_view input(argv[2]);
	if (task == "1.1")
	{
		return task_1_1(input);
	}
	if (task == "1.2")
	{
		return task_1_2(input);
	}
	if (task == "2.1")
	{
		return task_2_1(input);
	}
	if (task == "2.2")
	{
		return task_2_2(input);
	}
	if (task == "3.1")
	{
		return task_3_1(input);
	}
	if (task == "3.2")
	{
		return task_3_2(input);
	}
	return 1;
}
