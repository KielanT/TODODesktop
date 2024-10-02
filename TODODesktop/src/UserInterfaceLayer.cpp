#include "UserInterfaceLayer.h"
#include <iostream>

#include "curl/curl.h"
#include "json.hpp"


#include "Utility.h"
#include <imgui_internal.h>
#include "HTTPRequest.h"

void UserInterfaceLayer::OnAttach()
{
    GetList();
}

void UserInterfaceLayer::OnDetach()
{
    
}

void UserInterfaceLayer::OnUIRender()
{
    RenderList();

    RenderTask();

    RenderTaskProperties();




    //ImGui::ShowDemoWindow();
}

void UserInterfaceLayer::RenderList()
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
            {
                currentSelectedList = n;
                showTaskProperties = false;
                currentSelectedTask = -1;

            }

            if (ImGui::BeginPopupContextItem())
            {
                if (ImGui::Button("Delete"))
                {
                    DeleteList(n);
                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndPopup();
            }

            if (isSelected)
                ImGui::SetItemDefaultFocus();
        }

        ImGui::EndListBox();
    }

    ImGui::End();
}

void UserInterfaceLayer::RenderTask()
{
    ImGuiWindowClass window_class;
    window_class.DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_NoTabBar;
    ImGui::SetNextWindowClass(&window_class);
    ImGui::Begin("TWO");
    if (ImGui::Button("Add Task") && currentSelectedList > -1)
    {
        ImGui::OpenPopup("Add Task");
    }

    OnCreateAddTask();

    if (ImGui::BeginListBox("##TasksList", ImGui::GetContentRegionAvail()))
    {
        if (!ListVector.empty() && currentSelectedList > -1)
        {
            for (int n = 0; n < ListVector[currentSelectedList].TaskVector.size(); n++)
            {
                bool currentState = ListVector[currentSelectedList].TaskVector[n].Complete;

                ImGui::Checkbox(("###" + ListVector[currentSelectedList].TaskVector[n].Name).c_str(), &ListVector[currentSelectedList].TaskVector[n].Complete);

                if (currentState != ListVector[currentSelectedList].TaskVector[n].Complete)
                {
                    // Send to sql
                    OnCheckboxUpdate(n, ListVector[currentSelectedList].TaskVector[n].Complete);
                }

                ImGui::SameLine();

                const bool isSelected = (currentSelectedTask == n);
                if (ImGui::Selectable(ListVector[currentSelectedList].TaskVector[n].Name.c_str(), isSelected)) // TODO make a list item with checkbox
                    currentSelectedTask = n;

                if (ImGui::BeginPopupContextItem())
                {
                    if (ImGui::Button("Delete"))
                    {
                        DeleteTask(n);
                        ImGui::CloseCurrentPopup();
                    }
                    ImGui::EndPopup();
                }

                if (isSelected)
                    ImGui::SetItemDefaultFocus();
            }
        }

        ImGui::EndListBox();
    }
    ImGui::End();
}

void UserInterfaceLayer::RenderTaskProperties()
{
    if (currentSelectedTask > -1)
        showTaskProperties = true;

    if (showTaskProperties)
    {
        ImGuiWindowClass propertiesClass;
        propertiesClass.DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_NoTabBar;
        ImGui::SetNextWindowClass(&propertiesClass);

        static float b = 1.0f; //  test whatever color you need from imgui_demo.cpp e.g.
        static float c = 0.5f; // 
        static int i = 3;

        if (ImGui::Begin("##TaskProperties", &showTaskProperties))
        {
            ImGui::SameLine(ImGui::GetWindowWidth() - 30);
            ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(i / 7.0f, b, b, 0));
            if (ImGui::SmallButton("X"))
                showTaskProperties = false;
            ImGui::PopStyleColor();
            ImGui::Separator();

            ImGui::Text(ListVector[currentSelectedList].TaskVector[currentSelectedTask].Name.c_str());

            auto size = ImGui::GetContentRegionAvail();
            size.y = ImGui::CalcTextSize(ListVector[currentSelectedList].TaskVector[currentSelectedTask].Name.c_str()).y + 50;
            ImGui::Text("Description: ");
            ImGui::InputTextMultiline("Description", &ListVector[currentSelectedList].TaskVector[currentSelectedTask].TaskDesc, size);
            if (ImGui::IsItemDeactivatedAfterEdit())
            {
                OnDescUpdate(currentSelectedList, currentSelectedTask);
            }
        }
        ImGui::End();
    }

    if (showTaskProperties == false)
        currentSelectedTask = -1;
}

void UserInterfaceLayer::GetList()
{
    nlohmann::json jsonPayload;
    jsonPayload["email"] = "hello@world.com";
    nlohmann::json json = HTTPRequest::GET(m_URL + "/getLists", jsonPayload);

    if (json.contains("list"))
    {
        for (const auto& [title, data] : json["list"].items())
        {
            TODOList list(title);

            if (data.contains("tasks"))
            {
                for (const auto& task : data["tasks"])
                {
                    Task newTask(task["taskName"].get<std::string>());
                    newTask.Complete = task["complete"];
                    newTask.TaskDesc = task["taskDescription"];
                    list.TaskVector.push_back(newTask);
                }
            }

            ListVector.push_back(list);
        }
    }
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
            currentSelectedList++;
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
        nlohmann::json jsonPayload;
        jsonPayload["email"] = "hello@world.com";
        jsonPayload["name"] = result;

        nlohmann::json json = HTTPRequest::POST(m_URL + "/newList", jsonPayload);
        // TODO if response is good
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
        nlohmann::json jsonPayload;
        jsonPayload["email"] = "hello@world.com";
        jsonPayload["list"] = ListVector[currentSelectedList].Name;
        jsonPayload["name"] = result;

        nlohmann::json json = HTTPRequest::POST(m_URL + "/newTask", jsonPayload);
        // TODO if response is okay then add to the list
        ListVector[currentSelectedList].TaskVector.push_back(result);
    }
}

void UserInterfaceLayer::DeleteList(int index)
{
    currentSelectedList = index;

    nlohmann::json jsonPayload;
    jsonPayload["email"] = "hello@world.com";
    jsonPayload["name"] = ListVector[currentSelectedList].Name;

    nlohmann::json json = HTTPRequest::DELETEex(m_URL + "/deleteList", jsonPayload);
    
    if (json.contains("success") && json["success"])
    {
        ListVector.erase(ListVector.begin() + currentSelectedList);
        if (!ListVector.empty())
            currentSelectedList--;
        else
            currentSelectedList = -1;
    }
}

void UserInterfaceLayer::DeleteTask(int task)
{
    currentSelectedTask = task;

    nlohmann::json jsonPayload;
    jsonPayload["email"] = "hello@world.com";
    jsonPayload["list"] = ListVector[currentSelectedList].Name;
    jsonPayload["name"] = ListVector[currentSelectedList].TaskVector[currentSelectedTask].Name;

    nlohmann::json json = HTTPRequest::DELETEex(m_URL + "/deleteTask", jsonPayload);

    if (json.contains("success") && json["success"])
    {
        std::vector<Task>::iterator it = ListVector[currentSelectedList].TaskVector.begin();
        ListVector[currentSelectedList].TaskVector.erase(it + currentSelectedTask);
        currentSelectedTask = -1;
        showTaskProperties = false;
    }
}

void UserInterfaceLayer::OnCheckboxUpdate(int index, bool isChecked)
{
    nlohmann::json jsonPayload;
    jsonPayload["email"] = "hello@world.com";
    jsonPayload["list"] = ListVector[currentSelectedList].Name;
    jsonPayload["name"] = ListVector[currentSelectedList].TaskVector[index].Name;
    jsonPayload["complete"] = isChecked;

    HTTPRequest::PATCH(m_URL + "/updateTaskComplete", jsonPayload);
}

void UserInterfaceLayer::OnDescUpdate(int listIndex, int taskIndex)
{
    nlohmann::json jsonPayload;
    jsonPayload["email"] = "hello@world.com";
    jsonPayload["list"] = ListVector[listIndex].Name;
    jsonPayload["name"] = ListVector[listIndex].TaskVector[taskIndex].Name;
    jsonPayload["taskDesc"] = ListVector[listIndex].TaskVector[taskIndex].TaskDesc;
   
    HTTPRequest::PATCH(m_URL + "/updateTaskDesc", jsonPayload);
}
