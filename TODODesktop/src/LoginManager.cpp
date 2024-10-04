#include "LoginManager.h"
#include "crow.h"
#include "HTTPRequest.h"
#include "json.hpp"
#include <fstream>

std::string LoginManager::ClientId = "";
std::string LoginManager::ClientSecret = "";
std::string LoginManager::AccessToken = "";
User LoginManager::UserData{};


bool LoginManager::Login()
{
    // TODO read client id from file#
    // TODO explore doing it from the actual server

    crow::SimpleApp app;
  
    GetCredentials(ClientId, ClientSecret, "../../credentials.json");

    CROW_ROUTE(app, "/login")([&](const crow::request& req)
        {
            std::ostringstream url;
            url << "https://accounts.google.com/o/oauth2/v2/auth?"
                << "scope=email%20profile&"
                << "response_type=code&"
                //<< state=security_token%3D138r5719ru3e1%26url%3Dhttps%3A%2F%2Foauth2.example.com%2Ftoken&
                << "redirect_uri=http://localhost:3000/callback&"
                << "client_id=" << ClientId;

            crow::response res(302);
            res.add_header("Location", url.str());

            return res;
        });

    app.route_dynamic("/callback") ([&](const crow::request& req)
        {
            // TODO explore auth fail
            std::string code = req.url_params.get("code");

            AccessToken = HTTPRequest::ExchangeCodeforToken(ClientId, ClientSecret, code);
            GetUserData();

            app.stop();
            return crow::response("Successfully authenticated. \n");
        });

    std::string url = "http://localhost:3000/login";
    std::string command = "start " + url;
    system(command.c_str());
    app.port(3000).multithreaded().run();

    return true; // TODO detemine success
}

void LoginManager::GetCredentials(std::string& clientId, std::string& clientSecret, const std::string& path)
{
    std::fstream file(path);

    nlohmann::json data = nlohmann::json::parse(file);

    if (data.contains("installed"))
    {
        nlohmann::json installed = data["installed"];
        if (installed.contains("client_id"))
            clientId = installed["client_id"];

        if (installed.contains("client_secret"))
            clientSecret = installed["client_secret"];
    }
   
}

void LoginManager::GetUserData()
{
    nlohmann::json json = HTTPRequest::TestAPI(AccessToken);
   
    if (json.contains("id"))
        UserData.gID = json["id"];

    if (json.contains("given_name"))
        UserData.Name = json["given_name"];

    if (json.contains("email"))
        UserData.Email = json["email"];


}

