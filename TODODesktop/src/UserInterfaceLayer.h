#pragma once

#include "Walnut/Layer.h"
#include "imgui.h"

class UserInterfaceLayer : public Walnut::Layer
{
public:
	virtual void OnAttach() override;

	virtual void OnUIRender() override;

private:
	int GetCount();

	void UpdateCount();

private:
	int count = 0;
};