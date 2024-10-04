#include "HTTPRequest.h"

#include <iostream>

#include "curl/curl.h"

std::string HTTPRequest::GET(const std::string& url, const std::string postFields, const std::string& token)
{
	CURL* curl = curl_easy_init();

	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "GET");

	if(!postFields.empty())
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postFields.c_str());

	struct curl_slist* headers = nullptr;
	if (!token.empty())
	{
		headers = curl_slist_append(headers, ("Authorization: Bearer " + token).c_str());
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
	}

	std::string responseData{ "" };
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseData);

	CURLcode res = curl_easy_perform(curl);

	if (res != CURLE_OK)
	{
		std::cerr << "GET request failed: " << curl_easy_strerror(res) << std::endl;
	}

	if(headers)
		curl_slist_free_all(headers);

	curl_easy_cleanup(curl);

	return responseData;
}

std::string HTTPRequest::POST(const std::string& url, const std::string postFields)
{
	CURL* curl = curl_easy_init();

	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST");

	if (!postFields.empty())
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postFields.c_str());

	std::string responseData{ "" };
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseData);

	CURLcode res = curl_easy_perform(curl);

	if (res != CURLE_OK)
	{
		std::cerr << "POST request failed: " << curl_easy_strerror(res) << std::endl;
	}

	curl_easy_cleanup(curl);

	return responseData;
}


std::string HTTPRequest::DELETEex(const std::string& url, const std::string postFields)
{
	CURL* curl = curl_easy_init();

	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");

	if (!postFields.empty())
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postFields.c_str());

	std::string responseData{ "" };
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseData);

	CURLcode res = curl_easy_perform(curl);

	if (res != CURLE_OK)
	{
		std::cerr << "DELETE request failed: " << curl_easy_strerror(res) << std::endl;
	}

	curl_easy_cleanup(curl);

	return responseData;
}

void HTTPRequest::PATCH(const std::string& url, const std::string postFields)
{
	CURL* curl = curl_easy_init();

	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PATCH");

	if (!postFields.empty())
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postFields.c_str());

	std::string responseData{ "" };
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseData);

	CURLcode res = curl_easy_perform(curl);

	if (res != CURLE_OK)
	{
		std::cerr << "PATCH request failed: " << curl_easy_strerror(res) << std::endl;
	}

	curl_easy_cleanup(curl);

}
