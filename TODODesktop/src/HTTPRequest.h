#pragma once

#include <string>
#include "Utility.h"
#include "json.hpp"

//static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* userp)
//{
//	size_t total_size = size * nmemb;
//	userp->append(static_cast<char*>(contents), total_size);
//	return total_size;
//}

class HTTPRequest
{
public:
	static nlohmann::json GET(const std::string& url, nlohmann::json payload);
	static nlohmann::json POST(const std::string& url, nlohmann::json payload);
	static nlohmann::json DELETEex(const std::string& url, nlohmann::json payload);
	static void PATCH(const std::string& url, nlohmann::json payload); // TODO Add json for error checking

};

