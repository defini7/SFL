#pragma once

#pragma region license
/**
	BSD 3-Clause License

	Copyright (c) 2022, Alex
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

namespace def
{
	namespace net
	{
		template <typename T>
		class server;

		template <typename T>
		class connection : public std::enable_shared_from_this<connection<T>>
		{
		public:
			enum class side
			{
				server,
				client
			};

		public:
			connection(side parent, asio::io_context& context, asio::ip::tcp::socket socket, ts_deque<owned_message<T>>& messages)
				: m_context(context), m_socket(std::move(socket)), m_tsqMessagesIn(messages)
			{
				m_nOwner = parent;

				if (m_nOwner == side::server)
				{
					// Make random data (take a time so it will always be "random")
					// and send it to client to transorm and give it back
					m_nKnockOut = uint64_t(std::chrono::system_clock::now().time_since_epoch().count());

					// Pre-generate that so we can check it later
					m_nKnockCheck = encrypt(m_nKnockOut);
				}
				else
				{
					m_nKnockIn = 0;
					m_nKnockOut = 0;
				}
			}

			virtual ~connection() = default;

		public:
			uint32_t id() const
			{
				return m_nID;
			}

			void connect_client(server<T>* serv, uint32_t id = 0)
			{
				if (m_nOwner == side::server)
				{
					if (connected())
					{
						m_nID = id;

						WriteValidation();
						ReadValidation(serv);
					}
				}
			}

			void connect_server(const asio::ip::tcp::resolver::results_type& endpoints)
			{
				if (m_nOwner == side::client)
				{
					asio::async_connect(m_socket, endpoints,
						[this](asio::error_code ec, asio::ip::tcp::endpoint ep)
						{
							if (!ec)
							{
								// So let's start writing message
								//ReadHeader();

								ReadValidation();
							}
						}
					);
				}
			}

			void disconnect()
			{
				if (connected())
				{
					// If socket is open then just close it
					asio::post(m_context, [this]() { m_socket.close(); });
				}
			}

			bool connected() const
			{
				return m_socket.is_open();
			}

			void send(const message<T>& msg)
			{
				asio::post(m_context,
					[this, msg]()
					{
						bool bWasEmpty = m_tsqMessagesOut.empty();
						m_tsqMessagesOut.push_back(msg);
						
						if (bWasEmpty)
							WriteHeader();
					}
				);
			}

			uint64_t encrypt(uint64_t n)
			{
				uint64_t out;

				out = n ^ 0xF00BA4BA7;
				out = (out & 0xF0F0F0F0F0F0F0) >> 4 | (out & 0x0F0F0F0F0F0F0F) << 4;
				out = out ^ 0xF00BA71324;

				return out;
			}

		private:
			void WriteHeader()
			{
				// Writing message header
				asio::async_write(m_socket, asio::buffer(&m_tsqMessagesOut.front().header, sizeof(message_header<T>)),
					[this](asio::error_code ec, size_t length)
					{
						if (!ec)
						{
							// Check if message has a body
							if (m_tsqMessagesOut.front().body.size() > 0)
							{
								// so let's write body
								WriteBody();
							}
							else
							{
								// it doesn't have a message body
								// so just remove it
								m_tsqMessagesOut.pop_front();

								// If the deque isn't empty then there are more messages
								if (!m_tsqMessagesOut.empty())
									WriteHeader();
							}
						}
						else
						{
							// if it fails then just write fail reason
							// and close socket
							std::cerr << '[' << id() << "] " << ec.message() << std::endl;
							m_socket.close();
						}
					}
				);
			}

			void WriteBody()
			{
				asio::async_write(m_socket, asio::buffer(m_tsqMessagesOut.front().body.data(), m_tsqMessagesOut.front().body.size()),
					[this](asio::error_code ec, size_t length)
					{
						if (!ec)
						{
							// Remove message from queue
							// because sending was over
							m_tsqMessagesOut.pop_front();

							// If there are still any messages
							// then do message stuff again
							if (!m_tsqMessagesOut.empty())
								WriteHeader();
						}
						else
						{
							// if it fails then just write fail reason
							// and close socket
							std::cerr << '[' << id() << "] " << ec.message() << std::endl;
							m_socket.close();
						}
					}
				);
			}

			void ReadHeader()
			{
				asio::async_read(m_socket, asio::buffer(&m_msgCache.header, sizeof(message_header<T>)),
					[this](asio::error_code ec, size_t length)
					{
						if (!ec)
						{
							// Check if message has a body
							if (m_msgCache.header.size > 0)
							{
								// Then resize body vector and read it
								m_msgCache.body.resize(m_msgCache.header.size);
								ReadBody();
							}
							else
							{
								// else push it to incoming messages
								PushToIncomingQueue();
							}
						}
						else
						{
							// if it fails then just write fail reason
							// and close socket
							std::cerr << '[' << id() << "] " << ec.message() << std::endl;
							m_socket.close();
						}
					}
				);
			}

			void ReadBody()
			{
				asio::async_read(m_socket, asio::buffer(m_msgCache.body.data(), m_msgCache.body.size()),
					[this](asio::error_code ec, size_t length)
					{
						if (!ec)
						{
							// when bytes arrive
							// we just push message to incoming messages
							PushToIncomingQueue();
						}
						else
						{
							// if it fails then just write fail reason
							// and close socket
							std::cerr << '[' << id() << "] " << ec.message() << std::endl;
							m_socket.close();
						}
					}
				);
			}

			void PushToIncomingQueue()
			{
				// Convert to owned_message and save it in queue
				if (m_nOwner == side::server)
				{
					m_tsqMessagesIn.push_back({
						this->shared_from_this(),
						m_msgCache
					});
				}
				else
				{
					m_tsqMessagesIn.push_back({
						nullptr,
						m_msgCache
					});
				}

				// Prime context to recieve new message
				ReadHeader();
			}

			void ReadValidation(server<T>* serv = nullptr)
			{
				asio::async_read(m_socket, asio::buffer(&m_nKnockIn, sizeof(uint64_t)),
					[this, serv](std::error_code ec, std::size_t length)
					{
						if (!ec)
						{
							if (m_nOwner == side::server)
							{
								// Compare sent data to actual solution
								if (m_nKnockIn == m_nKnockCheck)
								{
									std::cout << "Client Validated" << std::endl;
									serv->OnClientValidated(this->shared_from_this());

									ReadHeader();
								}
								else
								{
									std::cout << "Client Disconnected (Validation failed)" << std::endl;
									m_socket.close();
								}
							}
							else
							{
								m_nKnockOut = encrypt(m_nKnockIn);

								WriteValidation();
							}
						}
						else
						{
							// Some biggerfailure occured
							std::cout << "Client Disconnected (ReadValidation)" << std::endl;
							m_socket.close();
						}
					});
			}

			void WriteValidation()
			{
				asio::async_write(m_socket, asio::buffer(&m_nKnockOut, sizeof(uint64_t)),
					[this](std::error_code ec, std::size_t length)
					{
						if (!ec)
						{
							// Validation data sent, clients should sit and wait
							// for a response (or a closure)
							if (m_nOwner == side::client)
								ReadHeader();
						}
						else
						{
							m_socket.close();
						}
					});
			}

		private:
			side m_nOwner = side::server;

			asio::ip::tcp::socket m_socket;
			asio::io_context& m_context;

			message<T> m_msgCache;

			ts_deque<message<T>> m_tsqMessagesOut;
			ts_deque<owned_message<T>>& m_tsqMessagesIn;

			uint32_t m_nID = 0;

			uint64_t m_nKnockIn = 0;
			uint64_t m_nKnockOut = 0;
			uint64_t m_nKnockCheck = 0;

		};
	}
}
