#pragma once

#include <string>

struct User
{
	std::string gID{ "" };
	std::string Email{ "" };
	std::string Name{ "" };
};

class LoginManager
{
public:
	static bool Login();

	static std::string ClientId;
	static std::string ClientSecret;
	static std::string AccessToken;
	static User UserData;

private:
	static void GetCredentials(std::string& clientId, std::string& clientSecret, const std::string& path);

	static void GetUserData();
};

