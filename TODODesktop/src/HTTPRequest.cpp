#include "HTTPRequest.h"

#include <iostream>

#include "curl/curl.h"

std::string HTTPRequest::GET(const std::string& url, nlohmann::json payload)
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

	if (res != CURLE_OK)
	{
		std::cerr << "GET request failed: " << curl_easy_strerror(res) << std::endl;
	}
	else
	{
		/*long responseCode;
		curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &responseCode);

		json["responseCode"] = responseCode;

		if (responseCode == 502)
		{
			json["error"] = "Server unavaliable!";
			curl_easy_cleanup(curl);

		}*/
	}


	curl_easy_cleanup(curl);

	return responseData;
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

std::string HTTPRequest::ExchangeCodeforToken(const std::string& clientId, const std::string& clientSecret, const std::string& code)
{
	std::string result{ "" };
	CURL* curl = curl_easy_init();

	std::string redirect_uri = "http://localhost:3000/callback"; // TODO change to server
	std::string grant_type = "authorization_code";

	std::string postFields = "code=" + code +
		"&client_id=" + clientId +
		"&client_secret=" + clientSecret +
		"&redirect_uri=" + redirect_uri +
		"&grant_type=" + grant_type;
							 
	std::string url = "https://oauth2.googleapis.com/token";

	struct curl_slist* headers = nullptr;
	headers = curl_slist_append(headers, "Content-Type: application/x-www-form-urlencoded");

	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST");
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postFields.c_str());
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

	std::string responseData{ "" };
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseData);

	CURLcode res = curl_easy_perform(curl);

	nlohmann::json json = nlohmann::json::parse(responseData);

	if (json.contains("access_token"))
		result = json["access_token"];

	if (res != CURLE_OK)
	{
		std::cerr << "POST request failed: " << curl_easy_strerror(res) << std::endl;
	}

	
	curl_slist_free_all(headers);
	curl_easy_cleanup(curl);

	return result;
}


nlohmann::json HTTPRequest::TestAPI(const std::string& token)
{
	CURL* curl = curl_easy_init();

	std::string url = "https://www.googleapis.com/oauth2/v2/userinfo";

	struct curl_slist* headers = nullptr;
	headers = curl_slist_append(headers, ("Authorization: Bearer " + token).c_str());

	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "GET");
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

	std::string responseData{ "" };
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseData);

	CURLcode res = curl_easy_perform(curl);

	nlohmann::json json = nlohmann::json::parse(responseData);;

	if (res != CURLE_OK)
	{
		std::cerr << "GET request failed: " << curl_easy_strerror(res) << std::endl;
	}

	
	curl_slist_free_all(headers);
	curl_easy_cleanup(curl);

	return json;
}
