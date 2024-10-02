#include "HTTPRequest.h"

#include <iostream>

#include "curl/curl.h"

nlohmann::json HTTPRequest::GET(const std::string& url, nlohmann::json payload)
{
	CURL* curl = curl_easy_init();

	std::string postFields = payload.dump();

	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "GET");
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postFields.c_str());

	std::string responseData{ "" };
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseData);

	CURLcode res = curl_easy_perform(curl);

	nlohmann::json json;


	if (res != CURLE_OK)
	{
		std::cerr << "GET request failed: " << curl_easy_strerror(res) << std::endl;
	}
	else
	{
		long responseCode;
		curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &responseCode);

		json["responseCode"] = responseCode;

		if (responseCode == 502)
		{
			json["error"] = "Server unavaliable!";
			curl_easy_cleanup(curl);
			return json; // TODO tell user that server is not on
		}
	}

	json = nlohmann::json::parse(responseData);

	curl_easy_cleanup(curl);

	return json;
}

nlohmann::json HTTPRequest::POST(const std::string& url, nlohmann::json payload)
{
	CURL* curl = curl_easy_init();

	std::string postFields = payload.dump();

	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST");
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postFields.c_str());

	std::string responseData{ "" };
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseData);

	CURLcode res = curl_easy_perform(curl);

	nlohmann::json json;
	if (res != CURLE_OK)
	{
		std::cerr << "POST request failed: " << curl_easy_strerror(res) << std::endl;
	}
	else
	{
		long responseCode;
		curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &responseCode);

		json["responseCode"] = responseCode;

		if (responseCode == 502)
		{
			json["error"] = "Server unavaliable!";
			curl_easy_cleanup(curl);
			return json; // TODO tell user that server is not on
		}
	}

	json = nlohmann::json::parse(responseData);

	curl_easy_cleanup(curl);

	return json;
}


nlohmann::json HTTPRequest::DELETEex(const std::string& url, nlohmann::json payload)
{
	CURL* curl = curl_easy_init();

	std::string postFields = payload.dump();

	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postFields.c_str());

	std::string responseData{ "" };
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseData);

	CURLcode res = curl_easy_perform(curl);

	nlohmann::json json;
	if (res != CURLE_OK)
	{
		std::cerr << "DELETE request failed: " << curl_easy_strerror(res) << std::endl;
	}
	else
	{
		long responseCode;
		curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &responseCode);

		json["responseCode"] = responseCode;

		if (responseCode == 502)
		{
			json["error"] = "Server unavaliable!";
			curl_easy_cleanup(curl);
			return json; // TODO tell user that server is not on
		}
	}

	json = nlohmann::json::parse(responseData);

	curl_easy_cleanup(curl);

	return json;
}

void HTTPRequest::PATCH(const std::string& url, nlohmann::json payload)
{
	CURL* curl = curl_easy_init();

	std::string postFields = payload.dump();

	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PATCH");
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
