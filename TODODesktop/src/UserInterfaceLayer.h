#pragma once

#include <thread>

#include "Walnut/Layer.h"
#include "imgui.h"

#include "websocketpp/client.hpp"
#include "websocketpp/config/asio_no_tls_client.hpp"

typedef websocketpp::client<websocketpp::config::asio_client> Client;

class UserInterfaceLayer : public Walnut::Layer
{
public:
	virtual void OnAttach() override;
	virtual void OnDetach() override;

	virtual void OnUIRender() override;

private:
	int GetCount();

	void UpdateCount();
	void ResetCount();

	void OnOpen(websocketpp::connection_hdl hdl);
	void OnMessage(websocketpp::connection_hdl, Client::message_ptr msg);

private:
	int count = 0;

	Client m_Client;
	std::thread NetworkThread;
	websocketpp::connection_hdl m_hdl;
};