#pragma once
#include <string>

static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* userp)
{
	size_t total_size = size * nmemb;
	userp->append(static_cast<char*>(contents), total_size);
	return total_size;
}