#pragma once

#include <string>
#include "json.hpp"

static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* userp)
{
	size_t total_size = size * nmemb;
	userp->append(static_cast<char*>(contents), total_size);
	return total_size;
}

class HTTPRequest
{
public:
	static std::string GET(const std::string& url, nlohmann::json payload);
	static nlohmann::json POST(const std::string& url, nlohmann::json payload);
	static nlohmann::json DELETEex(const std::string& url, nlohmann::json payload);
	static void PATCH(const std::string& url, nlohmann::json payload); // TODO Add json for error checking

	static std::string ExchangeCodeforToken(const std::string& clientId, const std::string& clientSecret, const std::string& code);

	static nlohmann::json TestAPI(const std::string& token); // TODO refactor this into get

};

