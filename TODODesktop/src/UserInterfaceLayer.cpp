#include "UserInterfaceLayer.h"
#include <iostream>

#include "curl/curl.h"
#include "json.hpp"


#include "Utility.h"

void OnClose(websocketpp::connection_hdl conn)
{
    std::cout << "WebSocket connection closed!!!!!!!!!" << std::endl; // When server closes the connection
}

void UserInterfaceLayer::OnAttach()
{
    count = GetCount();
    
    std::string uri = "ws://192.168.0.50:5000/ws";
    m_Client.set_access_channels(websocketpp::log::alevel::all);
    m_Client.clear_access_channels(websocketpp::log::alevel::frame_payload);
    m_Client.set_error_channels(websocketpp::log::elevel::all);

    m_Client.init_asio();
    //m_Client.start_perpetual();

    m_Client.set_open_handler(std::bind(&UserInterfaceLayer::OnOpen, this, std::placeholders::_1));
    m_Client.set_message_handler(std::bind(&UserInterfaceLayer::OnMessage, this, std::placeholders::_1, std::placeholders::_2));
    m_Client.set_close_handler(&OnClose);

    websocketpp::lib::error_code ec;
    Client::connection_ptr con = m_Client.get_connection(uri, ec);
    if (ec)
    {
        std::cout << "could not create connection because: " << ec.message() << std::endl;
        return;
    }

    m_Client.connect(con);

    NetworkThread = std::thread([this]()
        {
            m_Client.run();
        });
}

void UserInterfaceLayer::OnDetach()
{
    //m_Client.stop_perpetual();
    m_Client.close(m_hdl, websocketpp::close::status::going_away, "");


    if (NetworkThread.joinable())
        NetworkThread.join(); 

}

void UserInterfaceLayer::OnUIRender()
{
    ImGui::Begin("Hello");
    ImGui::Text("Clicked %i", count);

    if (ImGui::Button("Click me"))
    {
        UpdateCount();
    }

    if (ImGui::Button("Reset"))
    {
        ResetCount();
    }

    ImGui::End();

    //ImGui::ShowDemoWindow();
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

    curl_easy_cleanup(curl);
}

void UserInterfaceLayer::ResetCount()
{

    CURL* curl = curl_easy_init();
    curl_easy_setopt(curl, CURLOPT_URL, "http://192.168.0.50/reset");
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

void UserInterfaceLayer::OnOpen(websocketpp::connection_hdl hdl)
{
    std::cout << "Websocket Connection" << std::endl;
    m_hdl = hdl;
}

void UserInterfaceLayer::OnMessage(websocketpp::connection_hdl, Client::message_ptr msg)
{
    count = std::stoi(msg->get_payload());
}

