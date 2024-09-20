#include "UserInterfaceLayer.h"
#include <iostream>

#include "curl/curl.h"
#include "json.hpp"


#include "Utility.h"


void UserInterfaceLayer::OnAttach()
{
  
}

void UserInterfaceLayer::OnDetach()
{
    
}

void UserInterfaceLayer::OnUIRender()
{
    /*const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);

    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize;
    ImGui::Begin("main", NULL, window_flags);

    if (ImGui::Button("New List"))
    {

    }

    ImGui::End();*/

    ImGui::ShowDemoWindow();
}

void UserInterfaceLayer::CreateNewFile()
{
    CURL* curl = curl_easy_init();

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

    curl_easy_cleanup(curl);
}
