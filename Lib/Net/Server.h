#pragma once

#pragma region license
/**
	BSD 3-Clause License

	Copyright (c) 2022, �����
	All rights reserved.

	Redistribution and use in source and binary forms, with or without
	modification, are permitted provided that the following conditions are met:

	1. Redistributions of source code must retain the above copyright notice, this
	   list of conditions and the following disclaimer.

	2. Redistributions in binary form must reproduce the above copyright notice,
	   this list of conditions and the following disclaimer in the documentation
	   and/or other materials provided with the distribution.

	3. Neither the name of the copyright holder nor the names of its
	   contributors may be used to endorse or promote products derived from
	   this software without specific prior written permission.

	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
	AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
	IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
	DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
	FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
	DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
	SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
	CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
	OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
	OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#pragma endregion

#include "Common.h"
#include "TSDeque.h"
#include "Message.h"
#include "Connection.h"

namespace def
{
	namespace net
	{
		template <typename T>
		class server
		{
		public:
			server(uint16_t port) : m_acceptor(m_context, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port))
			{

			}

			virtual ~server()
			{
				stop();
			}

		public:
			bool start()
			{
				// instead of using asio::error_code, we can use try catch,
				// because asio::error_code is the std::error_code
				try
				{
					wait_connection();

					m_tContext = std::thread([this]() { m_context.run(); });
				}
				catch (std::exception& e)
				{
					std::cout << "[SERVER] Unable to connect: " << e.what() << std::endl;
					return false;
				}

				std::cout << "[SERVER] Started" << std::endl;
				return true;
			}

			void stop()
			{
				m_context.stop();

				if (m_tContext.joinable())
					m_tContext.join();

				std::cout << "[SERVER] Stopped" << std::endl;
			}

			void wait_connection()
			{
				m_acceptor.async_accept(
					[&](asio::error_code ec, asio::ip::tcp::socket socket)
					{
						if (!ec)
						{
							std::cout << "[SERVER] New connection: " << socket.remote_endpoint() << std::endl;

							std::shared_ptr<connection<T>> conn =
								std::make_shared<connection<T>>(
									connection<T>::side::server,
									m_context, std::move(socket), m_tsqMessagesIn
								);

							if (OnClientConnect(conn))
							{
								m_deqConnections.push_back(std::move(conn));

								m_deqConnections.back()->connect_client(this, m_nIDCounter++);

								std::cout << "[SERVER] Connection " << m_deqConnections.back()->id()
									<< " was approved" << std::endl;
							}
							else
							{
								// Deny connection if user wants that
								std::cout << "[SERVER] Connection" << conn << "was denied" << std::endl;
							}
						}
						else
						{
							// Can't connect to the server
							std::cout << "[SERVER] Connection error: " << ec.message() << std::endl;
						}

						wait_connection();
					}
				);
			}

			void send(std::shared_ptr<connection<T>> client, const message<T>& msg)
			{
				if (client && client->connected())
				{
					// If client is valid and still connected
					// then we send message
					client->send(msg);
				}
				else
				{
					// client isn't valid or isn't connected
					// so we just delete client

					OnClientDisconnect(client);

					client.reset();

					m_deqConnections.erase(
						std::remove(
							m_deqConnections.begin(),
							m_deqConnections.end(),
							client
						),
						m_deqConnections.end()
					);
				}
			}

			void send_all(const message<T>& msg, std::shared_ptr<connection<T>> ignored = nullptr)
			{
				bool bAnyInvalid = false;

				for (auto& client : m_deqConnections)
				{
					if (client && client->connected())
					{
						// Client is valid and connected
						// so we just send message if it's not an ignored client
						if (client != ignored)
							client->send(msg);
					}
					else
					{
						// client isn't valid or isn't connected
						// so we just delete client

						OnClientConnect(client);

						client.reset();

						// Set flag to optimize program, because
						// erase in deque is very expensive operation
						bAnyInvalid = true;
					}
				}

				if (bAnyInvalid)
				{
					// Just remove invalid clients
					m_deqConnections.erase(
						std::remove(
							m_deqConnections.begin(),
							m_deqConnections.end(),
							nullptr
						),
						m_deqConnections.end()
					);
				}
			}

		public:
			// User must call that to update state
			void update(size_t max = -1, bool wait = false)
			{
				// Store amount of messages
				size_t messages_count = 0;
				
				// While there are any messages and the message count is still good
				while (messages_count < max && !m_tsqMessagesIn.empty())
				{
					// Get message
					owned_message<T> msg = m_tsqMessagesIn.pop_front();

					OnMessage(msg.remote, msg.msg);

					messages_count++;
				}
			}

			// Called when a client connects
			virtual bool OnClientConnect(std::shared_ptr<connection<T>> client)
			{
				return false;
			}

			// Called when a client disconnects
			virtual void OnClientDisconnect(std::shared_ptr<connection<T>> client)
			{
				
			}
			
			virtual void OnClientValidated(std::shared_ptr<connection<T>> client)
			{
				
			}

			// Called when a message arrives
			virtual void OnMessage(std::shared_ptr<connection<T>> client, message<T>& msg)
			{
				
			}

		private:
			// Incoming messages
			ts_deque<owned_message<T>> m_tsqMessagesIn;

			// Will store all connections
			std::deque<std::shared_ptr<connection<T>>> m_deqConnections;

			// Unique ASIO context for each server
			asio::io_context m_context;
			
			// We must have thread to perform tasks
			std::thread m_tContext;

			// ASIO acceptor
			asio::ip::tcp::acceptor m_acceptor;

			// All IDs will start from this number
			uint32_t m_nIDCounter = 10000;
		};
	}
}
