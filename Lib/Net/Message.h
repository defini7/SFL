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

namespace def
{
	namespace net
	{
		template <typename T>
		struct message_header
		{
			T id{};
			uint32_t size = 0;
		};

		template <typename T>
		struct message
		{
			message_header<T> header{};
			std::vector<uint8_t> body;

			size_t size() const
			{
				return body.size();
			}

			template <typename DataType>
			void push(const DataType& data)
			{
				// Check if data is trivially copyable
				static_assert(std::is_standard_layout<DataType>::value, "Data is too complex");

				// Save vector size, so that would be index of new element
				size_t end = body.size();

				// Resize it to fit new element
				body.resize(end + sizeof(DataType));

				// Add new element
				memcpy(body.data() + end, &data, sizeof(DataType));

				// Update size
				header.size = size();
			}

			template <typename DataType>
			void get(DataType& data)
			{
				// Check if data is trivially copyable
				static_assert(std::is_standard_layout<DataType>::value, "Data is too complex");

				// Calculate new size of the vector
				size_t end = body.size() - sizeof(DataType);

				// Copy element in user variable
				memcpy(&data, body.data() + end, sizeof(DataType));

				// Shrink vector
				body.resize(end);

				// Update size
				header.size = size();
			}

			// More user friendly way to perform
			// pushing to and getting out of the vector
			template <typename DataType>
			message<T>& operator<<(const DataType& data)
			{
				push(data);
				return *this;
			}

			template <typename DataType>
			message<T>& operator>>(DataType& data)
			{
				get(data);
				return *this;
			}
		};

		template <typename T>
		class connection;

		template <typename T>
		struct owned_message
		{
			std::shared_ptr<connection<T>> remote = nullptr;
			message<T> msg;
		};
	}
}
