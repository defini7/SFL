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
#include <cstring>
#include <unordered_map>
#include <list>

#pragma endregion

namespace sfl
{
	struct Err
	{
		Err(bool _ok, std::string _msg) : ok(_ok), msg(_msg) {}
		operator bool() { return ok; }

		bool ok;
		std::string msg;
	};

	struct FlagUnit
	{
		std::string sValue;
		std::string sDescription;
		std::string sDefaultValue;
	};

	class Flag
	{
	public:
		Flag() = default;
		~Flag() = default;

	private:
		size_t Skip(std::string& sInput, char cEscapeChar)
		{
			size_t i = 0;
			while (sInput[i] == cEscapeChar) i++;
			return i;
		}

		void Trim(std::string& sInput)
		{
			while (sInput.back() == ' ')
				sInput.pop_back();

			while (sInput.front() == ' ')
				sInput.erase(0, 1);
		}

	public:
		Err Parse(int argc, const char** argv)
		{
			for (int i = 1; i < argc; i++)
				m_listContent.push_back(std::string(argv[i]));

			bool bNextShouldBeValue = false;
			bool bQuotes = false;

			std::string sStr = "";

			for (auto& c : m_listContent)
			{
				Trim(c);

				if (bQuotes)
				{
					if (c.back() == '"')
					{
						c.pop_back();
						bQuotes = false;
					}

					sStr += " " + c;
				}

				if (c[0] == '"')
				{
					c.erase(0, 1);
					sStr += c;
					bQuotes = true;
					continue;
				}

				if (bNextShouldBeValue)
				{
					if (c == "=")
						continue;

					m_mapContent[sLastKey].sValue = sStr.empty() ? c : sStr;
					sStr.clear();
					bNextShouldBeValue = false;

					continue;
				}

				std::string sFlag;
				std::string sValue;

				size_t nAssignAt = c.find('=');
				if (nAssignAt == std::string::npos)
				{
					// String's format is "-flag_name"
					sFlag = c;
					sValue = "";

					// We set a flag to true, to indicate
					// that next token should be value of flag
					bNextShouldBeValue = true;
				}
				else
				{
					// String's format is "-flag_name=10"
					sFlag = std::string(c.begin(), c.begin() + nAssignAt);
					sValue = std::string(c.begin() + nAssignAt + 1, c.end());
					Trim(sValue);
				}

				// If token starts with '-', then it should be a flag name
				if (sFlag[0] == '-' || (sFlag[0] == '-' && sFlag[1] == '-'))
				{
					std::string sName = std::string(sFlag.begin() + Skip(sFlag, '-'), sFlag.end());
					Trim(sName);
					sLastKey = sName;
					m_mapContent[sName].sValue = sStr.empty() ? sValue : sStr;
					sStr.clear();
				}
			}

			if (bQuotes)
				return Err(false, "\" was opened, but was never closed");

			return Err(true, "Ok");
		}

		int64_t Int(const std::string& sFlagName, int64_t nDefaultValue, std::string sDescription)
		{
			m_mapContent[sFlagName].sDefaultValue = std::to_string(nDefaultValue);
			m_mapContent[sFlagName].sDescription = sDescription;

			if (m_mapContent[sFlagName].sValue.empty())
				return nDefaultValue;

			return std::stoll(m_mapContent[sFlagName].sValue);
		}

		bool Bool(const std::string& sFlagName, bool bDefaultValue, std::string sDescription)
		{
			bool bHasFlag = (m_mapContent.count(sFlagName) > 0);

			m_mapContent[sFlagName].sDefaultValue = std::to_string(bDefaultValue);
			m_mapContent[sFlagName].sDescription = sDescription;

			if (!bHasFlag)
				return bDefaultValue;

			return true;
		}

		std::string Str(const std::string& sFlagName, std::string sDefaultValue, std::string sDescription)
		{
			m_mapContent[sFlagName].sDefaultValue = sDefaultValue;
			m_mapContent[sFlagName].sDescription = sDescription;

			if (m_mapContent[sFlagName].sValue.empty())
				return sDefaultValue;

			return m_mapContent[sFlagName].sValue;
		}

		void Help()
		{
			std::cout << "Info about flags: " << std::endl;

			for (const auto& flag : m_mapContent)
			{
				std::cout << "\tflag: " << flag.first
					<< ", default: " << flag.second.sDefaultValue
					<< ", description: " << flag.second.sDescription << std::endl;
			}
		}

	private:
		std::list<std::string> m_listContent;

		std::string sLastKey;
		std::unordered_map<std::string, FlagUnit> m_mapContent;

	};
}
