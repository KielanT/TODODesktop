#pragma once

#include <string>

// TODO update to a monostate class



struct User
{
	std::string gID{ "" };
	std::string Email{ "" };
	std::string Name{ "" };
};

class LoginManager
{
public:
	enum class TokenType
	{
		Auth,
		Refresh
	};

	static bool AutoLogin();
	static bool Login();

	static std::string ClientId;
	static std::string ClientSecret;
	static std::string AccessToken;
	static std::string RefreshToken;
	static User UserData;

private:
	static void GetCredentials(std::string& clientId, std::string& clientSecret, const std::string& path);
	static void SaveCredentials(const std::string& path);
	static void ExchangeToken(const std::string& code, const TokenType& token);
	static void GetUserData();
};

