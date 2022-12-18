#include <iostream>

#define SFL_NET
#include "SFL.h"

enum class MessageType : uint32_t
{
	ServerAccept,
	ServerDeny,
	ServerPing,
	MessageAll,
	ServerMessage
};

class CustomClient : public sfl::net::client<MessageType>
{
public:
	void PingServer()
	{
		sfl::net::message<MessageType> msg;
		msg.header.id = MessageType::ServerPing;

		auto time = std::chrono::system_clock::now();

		msg << time;
		send(msg);
	}
};

int main()
{
	CustomClient client;

	client.connect("127.0.0.1", 60000);

	bool bQuit = false;
	while (!bQuit)
	{
		if (GetForegroundWindow() == GetConsoleWindow())
		{
			if (GetAsyncKeyState('1') & 0x8000)
				client.PingServer();
		}

		if (client.connected())
		{
			if (!client.messages().empty())
			{
				sfl::net::message<MessageType> msg = client.messages().pop_front().msg;

				switch (msg.header.id)
				{
				case MessageType::ServerPing:
				{
					std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
					std::chrono::system_clock::time_point then;

					msg >> then;

					std::cout << "Ping: " << std::chrono::duration<double>(now - then).count() << std::endl;
				}
				}
			}
		}
		else
		{
			std::cout << "[SERVER] Stopped!";
			bQuit = true;
		}
	}

	return 0;
}
