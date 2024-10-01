#include "UserInterfaceLayer.h"
#include <iostream>

#include "curl/curl.h"
#include "json.hpp"


#include "Utility.h"
#include <imgui_internal.h>


void UserInterfaceLayer::OnAttach()
{
    CURL* curl = curl_easy_init();

    std::string str = m_URL + "/getLists";

    nlohmann::json jsonPayload;
    jsonPayload["email"] = "hello@world.com";
    std::string postFields = jsonPayload.dump();

    curl_easy_setopt(curl, CURLOPT_URL, str.c_str());
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "GET");
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postFields.c_str());

    std::string responseData{ "" };
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseData);

    CURLcode res = curl_easy_perform(curl);

    if (res != CURLE_OK)
    {
        std::cerr << "POST request failed: " << curl_easy_strerror(res) << std::endl;
    }
    else
    {
        long responseCode;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &responseCode);
        if (responseCode == 502)
            return; // TODO tell user that server is not on
    }

    nlohmann::json json = nlohmann::json::parse(responseData);

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
                    list.TaskVector.push_back(newTask);
                }
            }

            ListVector.push_back(list);
        }
    }

    curl_easy_cleanup(curl);
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
            {
                currentSelectedList = n;
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

    ImGui::SetNextWindowClass(&window_class);
    ImGui::Begin("TWO");
    if (ImGui::Button("Add Task"))
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
        
         CURL* curl = curl_easy_init();

         std::string str = m_URL + "/newList";

         nlohmann::json jsonPayload;
         jsonPayload["email"] = "hello@world.com";
         jsonPayload["name"] = result;
         std::string postFields = jsonPayload.dump();

         curl_easy_setopt(curl, CURLOPT_URL, str.c_str());
         curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST");
         curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postFields.c_str());

         std::string responseData{ "" };
         curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
         curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseData);

         CURLcode res = curl_easy_perform(curl);

         if (res != CURLE_OK)
         {
             std::cerr << "POST request failed: " << curl_easy_strerror(res) << std::endl;
         }

         // TODO if response is okay then add to the list
         ListVector.push_back(result);

         curl_easy_cleanup(curl);
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
        CURL* curl = curl_easy_init();

        std::string str = m_URL + "/newTask";

        nlohmann::json jsonPayload;
        jsonPayload["email"] = "hello@world.com";
        jsonPayload["list"] = ListVector[currentSelectedList].Name;
        jsonPayload["name"] = result;
        std::string postFields = jsonPayload.dump();

        curl_easy_setopt(curl, CURLOPT_URL, str.c_str());
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST");
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postFields.c_str());

        std::string responseData{ "" };
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseData);

        CURLcode res = curl_easy_perform(curl);

        if (res != CURLE_OK)
        {
            std::cerr << "POST request failed: " << curl_easy_strerror(res) << std::endl;
        }

        // TODO if response is okay then add to the list
        ListVector[currentSelectedList].TaskVector.push_back(result);;

        curl_easy_cleanup(curl);
        
    }
}

void UserInterfaceLayer::DeleteList(int index)
{
    currentSelectedList = index;

    CURL* curl = curl_easy_init();

    std::string str = m_URL + "/deleteList";

    nlohmann::json jsonPayload;
    jsonPayload["email"] = "hello@world.com";
    jsonPayload["name"] = ListVector[currentSelectedList].Name;
    std::string postFields = jsonPayload.dump();

    curl_easy_setopt(curl, CURLOPT_URL, str.c_str());
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postFields.c_str());

    std::string responseData{ "" };
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseData);

    CURLcode res = curl_easy_perform(curl);

    if (res != CURLE_OK)
    {
        std::cerr << "POST request failed: " << curl_easy_strerror(res) << std::endl;
    }

    curl_easy_cleanup(curl);

    nlohmann::json json = nlohmann::json::parse(responseData);
    
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

    CURL* curl = curl_easy_init();

    std::string str = m_URL + "/deleteTask";

    nlohmann::json jsonPayload;
    jsonPayload["email"] = "hello@world.com";
    jsonPayload["list"] = ListVector[currentSelectedList].Name;
    jsonPayload["name"] = ListVector[currentSelectedList].TaskVector[currentSelectedTask].Name;
    std::string postFields = jsonPayload.dump();

    curl_easy_setopt(curl, CURLOPT_URL, str.c_str());
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postFields.c_str());

    std::string responseData{ "" };
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseData);

    CURLcode res = curl_easy_perform(curl);

    if (res != CURLE_OK)
    {
        std::cerr << "POST request failed: " << curl_easy_strerror(res) << std::endl;
    }

    curl_easy_cleanup(curl);

    nlohmann::json json = nlohmann::json::parse(responseData);

    if (json.contains("success") && json["success"])
    {
        std::vector<Task>::iterator it = ListVector[currentSelectedList].TaskVector.begin();
        ListVector[currentSelectedList].TaskVector.erase(it + currentSelectedTask);
        currentSelectedTask = -1;
    }

    
}

void UserInterfaceLayer::OnCheckboxUpdate(int index, bool isChecked)
{
    CURL* curl = curl_easy_init();

    std::string str = m_URL + "/updateTaskComplete";

    nlohmann::json jsonPayload;
    jsonPayload["email"] = "hello@world.com";
    jsonPayload["list"] = ListVector[currentSelectedList].Name;
    jsonPayload["name"] = ListVector[currentSelectedList].TaskVector[index].Name;
    jsonPayload["complete"] = isChecked;
    std::string postFields = jsonPayload.dump();

    curl_easy_setopt(curl, CURLOPT_URL, str.c_str());
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PATCH");
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postFields.c_str());

    std::string responseData{ "" };
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseData);

    CURLcode res = curl_easy_perform(curl);

    if (res != CURLE_OK)
    {
        std::cerr << "PATCH request failed: " << curl_easy_strerror(res) << std::endl;
    }

    curl_easy_cleanup(curl);
}
