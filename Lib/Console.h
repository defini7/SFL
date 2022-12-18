#pragma once

#pragma region license
/***
*	BSD 3-Clause License
	Copyright (c) 2021, 2022 Alex
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
***/
#pragma endregion

#pragma region includes

#include <iostream>
#include <string>
#include <vector>

#pragma endregion

namespace sfl
{
	const std::string FG_BLACK = "\033[30m";
	const std::string FG_RED = "\033[31m";
	const std::string FG_GREEN = "\033[32m";
	const std::string FG_YELLOW = "\033[33m";
	const std::string FG_BLUE = "\033[34m";
	const std::string FG_MAGENTA = "\033[35m";
	const std::string FG_CYAN = "\033[36m";
	const std::string FG_WHITE = "\033[37m";
	const std::string FG_RESET = "\033[39m";
	
	const std::string BG_BLACK = "\033[40m";
	const std::string BG_RED = "\033[41m";
	const std::string BG_GREEN = "\033[42m";
	const std::string BG_YELLOW = "\033[43m";
	const std::string BG_BLUE = "\033[44m";
	const std::string BG_MAGENTA = "\033[45m";
	const std::string BG_CYAN = "\033[46m";
	const std::string BG_WHITE = "\033[47m";
	const std::string BG_RESET = "\033[49m";
	
	const std::string BOLD = "\033[1m";
	const std::string UNDERLINE = "\033[4m";
	const std::string RESET_ALL = "\033[0m";

	void Print(const std::string& sText, std::ostream& os = std::cout)
	{
		os << sText;
	}

	template <class It>
	void Print(It itBegin, It itEnd, std::ostream& os = std::cout)
	{
		for (It i = itBegin; i < itEnd; i++)
			std::cout << *i;
	}

	template <class It>
	void ContToStr(It itBegin, It itEnd, std::string& sOutput)
	{
		sOutput.clear();

		for (It i = itBegin; i < itEnd; i++)
			sOutput.append(1, *i);
	}
}
