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
#include <functional>
#include <fstream>
#include <list>
#include <stack>

#pragma endregion

namespace sfl
{
	class DataFile
	{
	public:
		DataFile() = default;
		~DataFile() = default;

	public:
		void SetString(const std::string& v)
		{
			sValue = v;
		}

		const std::string String() const
		{
			return sValue;
		}

		void SetInt(long long nValue)
		{
			SetString(std::to_string(nValue));
		}

		long long Int() const
		{
			return std::stoll(String());
		}

		void SetDecimal(long double dValue)
		{
			SetString(std::to_string(dValue));
		}

		long double Decimal() const
		{
			return std::stold(String());
		}

		void SetBool(const bool bValue)
		{
			SetString(std::to_string((int)bValue));
		}

		bool Bool() const
		{
			return (bool)std::stoll(String());
		}

		bool HasProperty(const std::string& name) const
		{
			// Check if object with that name already exists
			// for C++20 we can use    m_mapObjects.contains(name)
			return mapObjects.count(name) > 0;
		}

		DataFile& operator[](const std::string& key)
		{
			// Check if object with that name already exists
			// for C++20 we can use    m_mapObjects.contains(key)
			if (mapObjects.count(key) == 0)
			{
				// No, so create it

				// firstly set index to be accessed
				// by the name of the object
				mapObjects[key] = vecObjects.size();

				// then create empty object
				vecObjects.push_back({ key, DataFile() });
			}

			// If that exists then just return that object
			return vecObjects[mapObjects[key]].second;
		}

		DataFile& operator[](size_t index)
		{
			std::string i = std::to_string(index);

			// Check if object with that name already exists
			// for C++20 we can use    m_mapObjects.contains(key)
			if (mapObjects.count(i) == 0)
			{
				// No, so create it

				// firstly set index to be accessed
				// by the name of the object
				mapObjects[i] = vecObjects.size();

				// then create empty object
				vecObjects.push_back({ i, DataFile() });
			}

			// If that exists then just return that object
			return vecObjects[mapObjects[i]].second;
		}

		static bool Write(DataFile& dfDataFile, const std::string& sFileName)
		{
			std::function<void(std::ofstream&, DataFile&, size_t)> Write = [&](std::ofstream& os, DataFile& df, size_t tabs)
			{
				auto AddTabs = [&]()
				{
					for (size_t i = 0; i < tabs; i++)
						os << '\t';
				};

				// name, index
				for (auto& obj : df.vecObjects)
				{
					AddTabs();

					// Check if there are no other objects
					if (obj.second.vecObjects.empty())
					{
						// if it is, we should extract field name and value
						std::string& sName = obj.first;
						std::string& sValue = obj.second.sValue;

						// insert name and value
						os << sName << " = " << sValue << ";\n";
					}
					else
					{
						std::string& sName = obj.first;
						DataFile& dfChild = obj.second;

						os << sName << "\n";

						AddTabs();
						os << "(\n";

						// there are some objects, so parse them too
						Write(os, dfChild, tabs + 1);

						AddTabs();
						os << ')' << std::endl;
					}
				}
			};

			std::ofstream file(sFileName);

			// If file was opened without problems,
			// we just write into it
			if (file.is_open())
			{
				Write(file, dfDataFile, 0);

				file.close();
			}

			// ... else error was occured,
			// but we don't care about it's type
			return false;
		}

		static bool Read(DataFile& dfDataFile, const std::string& sFileName)
		{
			auto Trim = [](std::string& s)
			{
				s.erase(0, s.find_first_not_of(" \t\n\r\f\v"));
				s.erase(s.find_last_not_of(" \t\n\r\f\v") + 1);
			};

			std::ifstream fFile(sFileName);

			// If file was opened without problems,
			// we just read from it
			if (fFile.is_open())
			{
				std::string sFieldName = "";
				std::string sFieldValue = "";

				std::stack<std::reference_wrapper<DataFile>> stack;
				stack.push(dfDataFile);

				while (!fFile.eof())
				{
					// Get line from the file
					std::string sLine;
					std::getline(fFile, sLine);

					if (!sLine.empty())
						Trim(sLine);

					if (!sLine.empty())
					{
						// Find fields
						size_t i = sLine.find_first_of('=');
						if (i != std::string::npos)
						{
							sFieldName = sLine.substr(0, i);
							Trim(sFieldName);

							sFieldValue = sLine.substr(i + 1, sLine.length());
							Trim(sFieldValue);

							bool bQuotes = false;
							std::string sToken;

							for (const auto& c : sFieldValue)
							{
								if (c == '\"')
								{
									// start or end of the string
									bQuotes = !bQuotes;
								}
								else
								{
									if (bQuotes)
									{
										sToken += c;
									}
									else
									{
										if (c == ';')
										{
											Trim(sToken);

											// Add value to the vector of values
											stack.top().get()[sFieldName].SetString(sToken);

											// Reset token
											sToken.clear();
										}
										else
										{
											// if it's not a delimiter, so just add it
											sToken += c;
										}
									}
								}
							}

							if (!sToken.empty())
							{
								Trim(sToken);
								stack.top().get()[sFieldName].SetString(sToken);
							}
						}
						else // It's not a field
						{
							// then it could be an object
							if (sLine.front() == '{')
							{
								// Push it as an object to the stack
								stack.push(stack.top().get()[sFieldName]);
							}
							else
							{
								if (sLine.back() == '}')
								{
									// Object was already defined,
									// so remove it from the stack
									stack.pop();
								}
								else
								{
									sFieldName = sLine;
								}
							}
						}

					}
				}


				fFile.close();
				return true;
			}

			// ... else error was occured,
			// but we don't care about it's type
			return false;
		}

	public:
		// Just container to store values of light-weight type as string
		std::string sValue;

		// Store name of the object and object by itself
		std::vector<std::pair<std::string, DataFile>> vecObjects;

		// Store indecies that could be accessed by the name,
		// that's more user friendly way
		std::unordered_map<std::string, size_t>		  mapObjects;

	};
}
