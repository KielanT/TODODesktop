#pragma once

#include <string>
#include <vector>
#include <unordered_map>

#include "Walnut/Layer.h"
#include "imgui.h"

struct Task
{
	Task(std::string& name) : Name(name)
	{
	}

	std::string Name;
	bool Complete = false;
};

struct TODOList
{
	TODOList(const std::string& name) : Name(name)
	{}

	std::string Name{ "" };
	std::vector<Task> TaskVector;
};

class UserInterfaceLayer : public Walnut::Layer
{
public:
	virtual void OnAttach() override;
	virtual void OnDetach() override;

	virtual void OnUIRender() override;

private:
	void OnCreateNewList();
	void OnCreateAddTask();

	void DeleteList(int index);
	void DeleteTask(int task);

	void OnCheckboxUpdate(int index, bool isChecked);

private:
	const std::string m_URL{ "http://192.168.0.50" };

	std::vector<TODOList> ListVector;
	int currentSelectedList = 0; // TODO save in file
	int currentSelectedTask = -1;
};