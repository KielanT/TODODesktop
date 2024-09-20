#pragma once

#include <string>

#include "Walnut/Layer.h"
#include "imgui.h"

class UserInterfaceLayer : public Walnut::Layer
{
public:
	virtual void OnAttach() override;
	virtual void OnDetach() override;

	virtual void OnUIRender() override;

private:
	void CreateNewFile();


private:
	const std::string m_URL = "http://192.168.0.50";
};