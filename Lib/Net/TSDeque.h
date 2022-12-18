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
		class ts_deque
		{
		public:
			ts_deque() = default;
			ts_deque(const ts_deque<T>&) = delete;
			~ts_deque() { clear(); }

		public:
			const T& front()
			{
				std::scoped_lock lock(muxQueue);
				return deqQueue.front();
			}

			const T& back()
			{
				std::scoped_lock lock(muxQueue);
				return deqQueue.back();
			}

			void push_back(const T& value)
			{
				std::scoped_lock lock(muxQueue);
				deqQueue.emplace_back(std::move(value));

				// signal std::condition_variable to wake up thread
				std::unique_lock<std::mutex> ul(muxWaiting);
				cvWaiting.notify_one();
			}

			void push_front(const T& value)
			{
				std::scoped_lock lock(muxQueue);
				deqQueue.emplace_front(std::move(value));

				// signal std::condition_variable to wake up thread
				std::unique_lock<std::mutex> ul(muxWaiting);
				cvWaiting.notify_one();
			}

			bool empty()
			{
				std::scoped_lock lock(muxQueue);
				return deqQueue.empty();
			}

			size_t size()
			{
				std::scoped_lock lock(muxQueue);
				return deqQueue.size();
			}

			void clear()
			{
				std::scoped_lock lock(muxQueue);
				deqQueue.clear();
			}

			T pop_front()
			{
				std::scoped_lock lock(muxQueue);
				auto t = std::move(deqQueue.front());
				deqQueue.pop_front();
				return t;
			}

			T pop_back()
			{
				std::scoped_lock lock(muxQueue);
				auto t = std::move(deqQueue.back());
				deqQueue.pop_back();
				return t;
			}

			void wait()
			{
				// while there are no signals
				// we "freeze" the thread

				// there could be some errors,
				// but it doesn't really matter
				
				while (empty())
				{
					std::unique_lock<std::mutex> ul(muxWaiting);
					cvWaiting.wait(ul);
				}
			}

		private:
			std::mutex muxQueue;
			std::deque<T> deqQueue;

			std::condition_variable cvWaiting;
			std::mutex muxWaiting;
		};
	}
}
