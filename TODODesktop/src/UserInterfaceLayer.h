#pragma once

#include <string>
#include <vector>

#include "Walnut/Layer.h"
#include "imgui.h"

struct Task
{
	Task(std::string& name) : Name(name)
	{
	}

	std::string Name;
};

struct TODOList
{
	TODOList(std::string& name) : Name(name)
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
	void CreateNewFile();


private:
	const std::string m_URL{ "http://192.168.0.50" };

	std::vector<TODOList> ListVector;
	int currentSelectedList = 0; // TODO save in file
};