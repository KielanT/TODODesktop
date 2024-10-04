#pragma once

#include <string>


static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* userp)
{
	size_t total_size = size * nmemb;
	userp->append(static_cast<char*>(contents), total_size);
	return total_size;
}

class HTTPRequest
{
public:
	static std::string GET(const std::string& url, const std::string postFields = "", const std::string& token = "");
	static std::string POST(const std::string& url, const std::string postFields = "");
	static std::string DELETEex(const std::string& url, const std::string postFields = "");
	static void PATCH(const std::string& url, const std::string postFields = ""); // TODO Add json for error checking

};

