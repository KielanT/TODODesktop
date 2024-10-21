#include "LoginManager.h"
#include "crow.h"
#include "HTTPRequest.h"
#include "json.hpp"
#include <fstream>

std::string LoginManager::ClientId = "";
std::string LoginManager::ClientSecret = "";
std::string LoginManager::AccessToken = "";
std::string LoginManager::RefreshToken = "";
User LoginManager::UserData{};


bool LoginManager::AutoLogin()
{
    GetCredentials(ClientId, ClientSecret, "../../credentials.json");


    if (!RefreshToken.empty())
    {
        // TODO error check obtaining new token
        ExchangeToken(RefreshToken, TokenType::Refresh);
        GetUserData();

        return true;
    }
    return false;
}

bool LoginManager::Login()
{
    // TODO explore doing it from the actual server
    GetCredentials(ClientId, ClientSecret, "../../credentials.json");



    crow::SimpleApp app;

    CROW_ROUTE(app, "/login")([&](const crow::request& req)
        {
            std::ostringstream url;
            url << "https://accounts.google.com/o/oauth2/v2/auth?"
                << "scope=email%20profile&"
                << "response_type=code&"
                //<< state=security_token%3D138r5719ru3e1%26url%3Dhttps%3A%2F%2Foauth2.example.com%2Ftoken&
                << "redirect_uri=http://localhost:2000/callback&"
                << "client_id=" << ClientId;

            crow::response res(302);
            res.add_header("Location", url.str());

            return res;
        });

    app.route_dynamic("/callback") ([&](const crow::request& req)
        {
            // TODO explore auth fail
            std::string code = req.url_params.get("code");

            ExchangeToken(code, TokenType::Auth);
            GetUserData();

            app.stop();
            return crow::response("Successfully authenticated. \n");
        });

    std::string url = "http://localhost:2000/login";
    std::string command = "start " + url;
    system(command.c_str());
    app.port(2000).multithreaded().run();

    return true; // TODO detemine success
}

void LoginManager::GetCredentials(std::string& clientId, std::string& clientSecret, const std::string& path)
{
    std::fstream file(path);

    nlohmann::json data = nlohmann::json::parse(file);

    if (data.contains("installed")) // installed is just the in the file from google with the data we need
                                    // TODO create a file with encryption
    {
        nlohmann::json installed = data["installed"];
        if (installed.contains("client_id"))
            clientId = installed["client_id"];

        if (installed.contains("client_secret"))
            clientSecret = installed["client_secret"];

        if(installed.contains("refresh_token"))
            RefreshToken = installed["refresh_token"];
    }
    file.close();
}

void LoginManager::SaveCredentials(const std::string& path)
{
    if (RefreshToken.empty()) return; // Only saving refresh token for now

    // Read file
    std::fstream file(path);
    nlohmann::json data = nlohmann::json::parse(file);
    file.close();

    std::ofstream outFile(path);
    data["installed"]["refresh_token"] = LoginManager::RefreshToken;
    outFile << std::setw(4) << data << std::endl;
    outFile.close();

}

void LoginManager::ExchangeToken(const std::string& code, const TokenType& token)
{
    std::string postFields{ "" };
    if (token == TokenType::Auth)
    {
        postFields = "code=" + code +
            "&client_id=" + ClientId +
            "&client_secret=" + ClientSecret +
            "&redirect_uri=" + "http://localhost:2000/callback" +
            "&grant_type=authorization_code";
    }
    else if (token == TokenType::Refresh)
    {
        postFields = "client_id=" + ClientId +
            "&client_secret=" + ClientSecret +
            "&refresh_token=" + code +
            "&grant_type=refresh_token";
    }


    std::string response = HTTPRequest::POST("https://oauth2.googleapis.com/token", postFields);
    // TODO error checking
    nlohmann::json json = nlohmann::json::parse(response);
    if (json.contains("access_token"))
        AccessToken = json["access_token"];
    if (json.contains("refresh_token"))
    {
        RefreshToken = json["refresh_token"];
        SaveCredentials("../../credentials.json");
    }

}

void LoginManager::GetUserData()
{
    std::string url = "https://www.googleapis.com/oauth2/v2/userinfo";
    std::string response = HTTPRequest::GET(url, "", AccessToken);

    nlohmann::json json = nlohmann::json::parse(response);
    // TODO error checking

    if (json.contains("id"))
        UserData.gID = json["id"];

    if (json.contains("given_name"))
        UserData.Name = json["given_name"];

    if (json.contains("email"))
        UserData.Email = json["email"];


}

