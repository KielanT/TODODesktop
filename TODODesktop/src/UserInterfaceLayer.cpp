#include "UserInterfaceLayer.h"
#include <iostream>

#include "curl/curl.h"
#include "json.hpp"

#include <imgui_internal.h>
#include "HTTPRequest.h"
#include "LoginManager.h"


void UserInterfaceLayer::OnAttach()
{
    if (!LoginManager::AutoLogin())
        login = true;// Flag to login
    else
        GetList();
}

void UserInterfaceLayer::OnDetach()
{
    
}

void UserInterfaceLayer::OnUIRender()
{
    RenderLogin();

    RenderList();

    RenderTask();

    RenderTaskProperties();

    //ImGui::ShowDemoWindow();
}

void UserInterfaceLayer::RenderLogin()
{
    if (!login) return;

    static bool isFirstCall{ true };
    static bool isSuccess{ false };
    static std::string titleText{ "Please Login!" };

    if (isFirstCall)
    {
        ImGui::OpenPopup("LoginModal");
        isFirstCall = false;
    }

    ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove; // TODO make pretty
    if (ImGui::BeginPopupModal("LoginModal", NULL, flags))
    {
        ImGui::Text(titleText.c_str());

        if (!isSuccess)
        {
            if (ImGui::Button("Login"))
            {
                isSuccess = LoginManager::Login();
            }
        }
        else
        {
            if (ImGui::Button("Continue"))
            {
                login = false;
                ImGui::CloseCurrentPopup();
            }

            static bool IsFirstTimeLogin = IsFirstTime();

            if (IsFirstTimeLogin)
            {
                titleText = "Welcome " + LoginManager::UserData.Name;
                CreateUser();
            }
            else
            {
                titleText = "Welcome back " + LoginManager::UserData.Name;
                GetList();
            }
        }

        ImGui::EndPopup();
    }

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

bool UserInterfaceLayer::IsFirstTime()
{
    nlohmann::json jsonPayload;
    jsonPayload["gID"] = LoginManager::UserData.gID;
    std::string response = HTTPRequest::GET(m_URL + "/doesUserExist", jsonPayload.dump());
    // TODO use response for error check 

    nlohmann::json json = nlohmann::json::parse(response);

    if (json.contains("exist"))
        return !json["exist"].get<bool>(); // exist returns false if the user does not exist, we want to then return true to show that it is the first time


    return false;
}

void UserInterfaceLayer::CreateUser()
{
    nlohmann::json jsonPayload;
    // TODO get response if failed
    jsonPayload["name"] = LoginManager::UserData.Name;
    jsonPayload["email"] = LoginManager::UserData.Email;
    jsonPayload["id"] = LoginManager::UserData.gID;
    std::string response = HTTPRequest::POST(m_URL + "/createUser", jsonPayload.dump());
    // TODO check
}

void UserInterfaceLayer::GetList()
{
    nlohmann::json jsonPayload;
    jsonPayload["email"] = LoginManager::UserData.Email;
    jsonPayload["id"] = LoginManager::UserData.gID;
    std::string response = HTTPRequest::GET(m_URL + "/getLists", jsonPayload.dump());
    // TODO check
    nlohmann::json json = nlohmann::json::parse(response);
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
        jsonPayload["id"] = LoginManager::UserData.gID;
        jsonPayload["email"] = LoginManager::UserData.Email;
        jsonPayload["name"] = result;

       std::string response = HTTPRequest::POST(m_URL + "/newList", jsonPayload.dump());
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
        jsonPayload["email"] = LoginManager::UserData.Email;
        jsonPayload["id"] = LoginManager::UserData.gID;
        jsonPayload["list"] = m_TODOList.ListVector[m_TODOList.currentSelectedList].Name;
        jsonPayload["name"] = result;

        std::string response = HTTPRequest::POST(m_URL + "/newTask", jsonPayload.dump());
        // TODO if response is okay then add to the list
        m_TODOList.ListVector[m_TODOList.currentSelectedList].TaskVector.push_back(result);
    }
}

void UserInterfaceLayer::DeleteList(int index)
{
    m_TODOList.currentSelectedList = index;

    nlohmann::json jsonPayload;
    jsonPayload["email"] = LoginManager::UserData.Email;
    jsonPayload["id"] = LoginManager::UserData.gID;
    jsonPayload["name"] = m_TODOList.ListVector[m_TODOList.currentSelectedList].Name;

    std::string response = HTTPRequest::DELETEex(m_URL + "/deleteList", jsonPayload.dump());
    // TODO error check
    nlohmann::json json = nlohmann::json::parse(response);

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
    jsonPayload["email"] = LoginManager::UserData.Email;
    jsonPayload["id"] = LoginManager::UserData.gID;
    jsonPayload["list"] = m_TODOList.ListVector[m_TODOList.currentSelectedList].Name;
    jsonPayload["name"] = m_TODOList.ListVector[m_TODOList.currentSelectedList].TaskVector[m_TODOList.currentSelectedTask].Name;

    std::string response = HTTPRequest::DELETEex(m_URL + "/deleteTask", jsonPayload.dump());
    nlohmann::json json = nlohmann::json::parse(response);

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
    jsonPayload["email"] = LoginManager::UserData.Email;
    jsonPayload["id"] = LoginManager::UserData.gID;
    jsonPayload["list"] = m_TODOList.ListVector[m_TODOList.currentSelectedList].Name;
    jsonPayload["name"] = m_TODOList.ListVector[m_TODOList.currentSelectedList].TaskVector[index].Name;
    jsonPayload["complete"] = isChecked;

    HTTPRequest::PATCH(m_URL + "/updateTaskComplete", jsonPayload.dump());
}

void UserInterfaceLayer::OnDescUpdate(int listIndex, int taskIndex)
{
    nlohmann::json jsonPayload;
    jsonPayload["email"] = LoginManager::UserData.Email;
    jsonPayload["id"] = LoginManager::UserData.gID;
    jsonPayload["list"] = m_TODOList.ListVector[listIndex].Name;
    jsonPayload["name"] = m_TODOList.ListVector[listIndex].TaskVector[taskIndex].Name;
    jsonPayload["taskDesc"] = m_TODOList.ListVector[listIndex].TaskVector[taskIndex].TaskDesc;
   
    HTTPRequest::PATCH(m_URL + "/updateTaskDesc", jsonPayload.dump());
}
