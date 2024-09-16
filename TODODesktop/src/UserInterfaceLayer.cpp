#include "UserInterfaceLayer.h"
#include <iostream>

#include "curl/curl.h"
#include "json.hpp"

#include "Utility.h"


void UserInterfaceLayer::OnAttach()
{
    count = GetCount();
}

void UserInterfaceLayer::OnUIRender()
{
    ImGui::Begin("Hello");
    ImGui::Text("Clicked %i", count);

    if (ImGui::Button("Click me"))
    {
        UpdateCount();
    }
    if (ImGui::Button("Refresh"))
    {
        count = GetCount();
    }

    ImGui::End();

    ImGui::ShowDemoWindow();
}

int UserInterfaceLayer::GetCount()
{
    CURL* curl = curl_easy_init();
    curl_easy_setopt(curl, CURLOPT_URL, "http://192.168.0.50/count");
    curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);

    std::string responseData{ "" };
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseData);

    CURLcode res = curl_easy_perform(curl);

    if (res != CURLE_OK)
    {
        std::cerr << "GET request failed: " << curl_easy_strerror(res) << std::endl;
    }

    curl_easy_cleanup(curl);

    nlohmann::json json = nlohmann::json::parse(responseData);
    
    if (json.contains("count"))
    {
        return json["count"];
    }

    return 0;
}

void UserInterfaceLayer::UpdateCount()
{
    CURL* curl = curl_easy_init();
    curl_easy_setopt(curl, CURLOPT_URL, "http://192.168.0.50/update");
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST");

    std::string responseData{ "" };
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseData);

    CURLcode res = curl_easy_perform(curl);

    if (res != CURLE_OK)
    {
        std::cerr << "POST request failed: " << curl_easy_strerror(res) << std::endl;
    }

    count++;

    curl_easy_cleanup(curl);
}

