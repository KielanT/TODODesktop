#pragma once

#include <string>
#include <vector>

struct Task
{
	Task(std::string& name) : Name(name)
	{
	}

	std::string Name{ "" };
	std::string TaskDesc{ "" };
	bool Complete{ false };
};

struct TODOList
{
	TODOList(const std::string& name) : Name(name)
	{
	}

	std::string Name{ "" };
	std::vector<Task> TaskVector;
};


class TODOListManager
{
public:
	std::vector<TODOList> ListVector;
	int currentSelectedList = -1; // TODO save in file
	int currentSelectedTask = -1;
};

