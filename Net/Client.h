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

namespace def
{
	namespace net
	{
		template <typename T>
		class client
		{
		public:
			client() {}

			virtual ~client()
			{
				disconnect();
			}

		public:
			bool connect(const std::string& host, const uint16_t port)
			{
				// instead of using asio::error_code, we can use try catch,
				// because asio::error_code is the std::error_code
				try
				{
					asio::ip::tcp::resolver resolver(m_context);
					m_endpoints = resolver.resolve(host, std::to_string(port));

					m_connection = std::make_unique<connection<T>>(
						connection<T>::side::client,
						m_context,
						asio::ip::tcp::socket(m_context),
						m_tsqMessagesIn
					);

					m_connection->connect_server(m_endpoints);

					m_tContext = std::thread([&]() { m_context.run(); });
				}
				catch (std::exception& e)
				{
					std::cerr << "[CLIENT] Unable to connect to " <<
						host << ": " << e.what() << std::endl;

					return false;
				}

				std::cout << "[CLIENT] Connected!" << std::endl;
				return false;
			}

			void disconnect()
			{
				// If connection is valid then we disconnect
				if (connected())
					m_connection->disconnect();

				m_context.stop();

				if (m_tContext.joinable())
					m_tContext.join();
			}

			bool connected()
			{
				if (m_connection)
					return m_connection->connected();
				
				return false;
			}

			void send(const message<T>& msg)
			{
				if (connected())
					m_connection->send(msg);
			}

			ts_deque<owned_message<T>>& messages()
			{
				return m_tsqMessagesIn;
			}
			
		private:
			// We need unique context for each client
			asio::io_context m_context;

			// We need thread to perform tasks
			std::thread m_tContext;

			// Also we must have connection to manage data
			std::unique_ptr<connection<T>> m_connection;

			// Just incoming messages
			ts_deque<owned_message<T>> m_tsqMessagesIn;

			// Save endpoints
			asio::ip::tcp::resolver::results_type m_endpoints;
		};
	}
}
