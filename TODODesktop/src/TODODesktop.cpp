#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"

#include "Walnut/Image.h"

#include "UserInterfaceLayer.h"

Walnut::Application* Walnut::CreateApplication(int argc, char** argv)
{
	Walnut::ApplicationSpecification spec;
	spec.Name = "TODO List Desktop";

	Walnut::Application* app = new Walnut::Application(spec);
	app->PushLayer<UserInterfaceLayer>();
	//app->SetMenubarCallback([app]()
	//{
	//	if (ImGui::BeginMenu("File"))
	//	{
	//		if (ImGui::MenuItem("Exit"))
	//		{
	//			app->Close();
	//		}
	//		ImGui::EndMenu();
	//	}
	//});
	return app;
}