#include <iostream>
#include <net/net.h>

enum class MessageType : uint32_t
{
	ServerAccept,
	ServerDeny,
	ServerPing,
	MessageAll,
	ServerMessage
};

class CustomServer : public def::net::server<MessageType>
{
public:
	CustomServer(uint16_t port) : def::net::server<MessageType>(port)
	{

	}

protected:
	bool OnClientConnect(std::shared_ptr<def::net::connection<MessageType>> client) override
	{
		return true;
	}

	void OnClientDisconnect(std::shared_ptr<def::net::connection<MessageType>> client) override
	{

	}

	void OnMessage(std::shared_ptr<def::net::connection<MessageType>> client, def::net::message<MessageType>& msg) override
	{
		switch (msg.header.id)
		{
		case MessageType::ServerPing:
		{
			std::cout << '[' << client->id() << "]: Server pinged" << std::endl;

			client->send(msg);
		}
		break;

		}
	}

};

int main()
{
	CustomServer server(60000);

	if (!server.start())
		return 1;

	while (1)
	{
		server.update(-1, true);
	}

	return 0;
}
