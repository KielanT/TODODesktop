#include "UserInterfaceLayer.h"
#include <iostream>

#include "curl/curl.h"
#include "json.hpp"

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
        for (int n = 0; n < m_TODOList.ListVector.size(); n++)
        {
            const bool isSelected = (m_TODOList.currentSelectedList == n);
            if (ImGui::Selectable(m_TODOList.ListVector[n].Name.c_str(), isSelected))
            {
                m_TODOList.currentSelectedList = n;
                showTaskProperties = false;
                m_TODOList.currentSelectedTask = -1;

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
    if (ImGui::Button("Add Task") && m_TODOList.currentSelectedList > -1)
    {
        ImGui::OpenPopup("Add Task");
    }

    OnCreateAddTask();

    if (ImGui::BeginListBox("##TasksList", ImGui::GetContentRegionAvail()))
    {
        if (!m_TODOList.ListVector.empty() && m_TODOList.currentSelectedList > -1)
        {
            for (int n = 0; n < m_TODOList.ListVector[m_TODOList.currentSelectedList].TaskVector.size(); n++)
            {
                bool currentState = m_TODOList.ListVector[m_TODOList.currentSelectedList].TaskVector[n].Complete;

                ImGui::Checkbox(("###" + m_TODOList.ListVector[m_TODOList.currentSelectedList].TaskVector[n].Name).c_str(), &m_TODOList.ListVector[m_TODOList.currentSelectedList].TaskVector[n].Complete);

                if (currentState != m_TODOList.ListVector[m_TODOList.currentSelectedList].TaskVector[n].Complete)
                {
                    // Send to sql
                    OnCheckboxUpdate(n, m_TODOList.ListVector[m_TODOList.currentSelectedList].TaskVector[n].Complete);
                }

                ImGui::SameLine();

                const bool isSelected = (m_TODOList.currentSelectedTask == n);
                if (ImGui::Selectable(m_TODOList.ListVector[m_TODOList.currentSelectedList].TaskVector[n].Name.c_str(), isSelected)) // TODO make a list item with checkbox
                    m_TODOList.currentSelectedTask = n;

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
    if (m_TODOList.currentSelectedTask > -1)
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

            ImGui::Text(m_TODOList.ListVector[m_TODOList.currentSelectedList].TaskVector[m_TODOList.currentSelectedTask].Name.c_str());

            auto size = ImGui::GetContentRegionAvail();
            size.y = ImGui::CalcTextSize(m_TODOList.ListVector[m_TODOList.currentSelectedList].TaskVector[m_TODOList.currentSelectedTask].Name.c_str()).y + 50;
            ImGui::Text("Description: ");
            ImGui::InputTextMultiline("Description", &m_TODOList.ListVector[m_TODOList.currentSelectedList].TaskVector[m_TODOList.currentSelectedTask].TaskDesc, size);
            if (ImGui::IsItemDeactivatedAfterEdit())
            {
                OnDescUpdate(m_TODOList.currentSelectedList, m_TODOList.currentSelectedTask);
            }
        }
        ImGui::End();
    }

    if (showTaskProperties == false)
        m_TODOList.currentSelectedTask = -1;
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

            m_TODOList.ListVector.push_back(list);
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
            m_TODOList.currentSelectedList++;
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
        m_TODOList.ListVector.push_back(result);

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
        jsonPayload["list"] = m_TODOList.ListVector[m_TODOList.currentSelectedList].Name;
        jsonPayload["name"] = result;

        nlohmann::json json = HTTPRequest::POST(m_URL + "/newTask", jsonPayload);
        // TODO if response is okay then add to the list
        m_TODOList.ListVector[m_TODOList.currentSelectedList].TaskVector.push_back(result);
    }
}

void UserInterfaceLayer::DeleteList(int index)
{
    m_TODOList.currentSelectedList = index;

    nlohmann::json jsonPayload;
    jsonPayload["email"] = "hello@world.com";
    jsonPayload["name"] = m_TODOList.ListVector[m_TODOList.currentSelectedList].Name;

    nlohmann::json json = HTTPRequest::DELETEex(m_URL + "/deleteList", jsonPayload);
    
    if (json.contains("success") && json["success"])
    {
        m_TODOList.ListVector.erase(m_TODOList.ListVector.begin() + m_TODOList.currentSelectedList);
        if (!m_TODOList.ListVector.empty())
            m_TODOList.currentSelectedList--;
        else
            m_TODOList.currentSelectedList = -1;
    }
}

void UserInterfaceLayer::DeleteTask(int task)
{
    m_TODOList.currentSelectedTask = task;

    nlohmann::json jsonPayload;
    jsonPayload["email"] = "hello@world.com";
    jsonPayload["list"] = m_TODOList.ListVector[m_TODOList.currentSelectedList].Name;
    jsonPayload["name"] = m_TODOList.ListVector[m_TODOList.currentSelectedList].TaskVector[m_TODOList.currentSelectedTask].Name;

    nlohmann::json json = HTTPRequest::DELETEex(m_URL + "/deleteTask", jsonPayload);

    if (json.contains("success") && json["success"])
    {
        std::vector<Task>::iterator it = m_TODOList.ListVector[m_TODOList.currentSelectedList].TaskVector.begin();
        m_TODOList.ListVector[m_TODOList.currentSelectedList].TaskVector.erase(it + m_TODOList.currentSelectedTask);
        m_TODOList.currentSelectedTask = -1;
        showTaskProperties = false;
    }
}

void UserInterfaceLayer::OnCheckboxUpdate(int index, bool isChecked)
{
    nlohmann::json jsonPayload;
    jsonPayload["email"] = "hello@world.com";
    jsonPayload["list"] = m_TODOList.ListVector[m_TODOList.currentSelectedList].Name;
    jsonPayload["name"] = m_TODOList.ListVector[m_TODOList.currentSelectedList].TaskVector[index].Name;
    jsonPayload["complete"] = isChecked;

    HTTPRequest::PATCH(m_URL + "/updateTaskComplete", jsonPayload);
}

void UserInterfaceLayer::OnDescUpdate(int listIndex, int taskIndex)
{
    nlohmann::json jsonPayload;
    jsonPayload["email"] = "hello@world.com";
    jsonPayload["list"] = m_TODOList.ListVector[listIndex].Name;
    jsonPayload["name"] = m_TODOList.ListVector[listIndex].TaskVector[taskIndex].Name;
    jsonPayload["taskDesc"] = m_TODOList.ListVector[listIndex].TaskVector[taskIndex].TaskDesc;
   
    HTTPRequest::PATCH(m_URL + "/updateTaskDesc", jsonPayload);
}
