#include "UserInterfaceLayer.h"
#include <iostream>

#include "curl/curl.h"
#include "json.hpp"


#include "Utility.h"
#include <imgui_internal.h>


void UserInterfaceLayer::OnAttach()
{
}

void UserInterfaceLayer::OnDetach()
{
    
}

void UserInterfaceLayer::OnUIRender()
{

    ImGuiWindowClass window_class;
    window_class.DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_NoTabBar;
    ImGui::SetNextWindowClass(&window_class);

    ImGui::Begin("main");
    if (ImGui::Button("New List"))
    {
       ImGui::OpenPopup("Create New List");
    }

    OnCreateNewList();

    if (ImGui::BeginListBox("##ListsList", ImGui::GetContentRegionAvail()))
    {
        for (int n = 0; n < ListVector.size(); n++)
        {
            const bool isSelected = (currentSelectedList == n);
            if (ImGui::Selectable(ListVector[n].Name.c_str(), isSelected))
                currentSelectedList = n;

            if (isSelected)
                ImGui::SetItemDefaultFocus();
        }

        ImGui::EndListBox();
    }


    ImGui::End();

    ImGui::SetNextWindowClass(&window_class);
    ImGui::Begin("TWO");
    if (ImGui::Button("Add Task"))
    {
        ImGui::OpenPopup("Add Task");
    }

    OnCreateAddTask();

    if (ImGui::BeginListBox("##TasksList", ImGui::GetContentRegionAvail()))
    {
        if (!ListVector.empty())
        {
            for (int n = 0; n < ListVector[currentSelectedList].TaskVector.size(); n++)
            {
                ImGui::Selectable(ListVector[currentSelectedList].TaskVector[n].Name.c_str(), false); // TODO make a list item with checkbox
            }
        }

        ImGui::EndListBox();
    }

    ImGui::End();

   //ImGui::ShowDemoWindow();
}

void UserInterfaceLayer::OnCreateNewList()
{
    std::string result = "";

    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    if (ImGui::BeginPopupModal("Create New List", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove))
    {
        ImGui::Text("Choose a name");
        ImGui::Separator();

        
        static char buf[64] = "";
        ImGui::InputText("###", buf, 64);

        if (ImGui::Button("Add"))
        {
            result = buf; 
            buf[0] = '\0';
            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine();

        if (ImGui::Button("Cancel"))
        {
            buf[0] = '\0';
            ImGui::CloseCurrentPopup();
        }
        

        ImGui::EndPopup();
    }
    
    if (!result.empty())
    {
        // Create list
        ListVector.push_back(result);
    }
}

void UserInterfaceLayer::OnCreateAddTask()
{
    std::string result = "";

    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    if (ImGui::BeginPopupModal("Add Task", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove))
    {
        ImGui::Text("Choose a name");
        ImGui::Separator();


        static char buf[64] = "";
        ImGui::InputText("###", buf, 64);

        if (ImGui::Button("Add"))
        {
            result = buf;
            buf[0] = '\0';
            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine();

        if (ImGui::Button("Cancel"))
        {
            buf[0] = '\0';
            ImGui::CloseCurrentPopup();
        }


        ImGui::EndPopup();
    }

    if (!result.empty())
    {
        // Create list
        ListVector[currentSelectedList].TaskVector.push_back(result);
    }
}

void UserInterfaceLayer::CreateNewFile()
{
    /*CURL* curl = curl_easy_init();

    std::string str = m_URL + "/newList";

    curl_easy_setopt(curl, CURLOPT_URL, str.c_str());
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST");

    std::string responseData{ "" };
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseData);

    CURLcode res = curl_easy_perform(curl);

    if (res != CURLE_OK)
    {
        std::cerr << "POST request failed: " << curl_easy_strerror(res) << std::endl;
    }

    curl_easy_cleanup(curl);*/
}
