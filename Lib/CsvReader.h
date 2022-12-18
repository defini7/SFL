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
**/
#pragma endregion

#pragma region includes

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#pragma endregion

namespace sfl
{
	class CsvReader
	{
	public:
		CsvReader() = default;
		CsvReader(std::string sFileName)
		{
			SetFileName(sFileName);
			Load();
		}

	private:
		std::ifstream m_ifCsvFile;
		std::string m_sFileName;

		struct Cell
		{
			int nRow;
			int nCol;

			std::string sValue;
		};

		std::vector<Cell> m_vecCells;

		void Split(std::string sInput, std::vector<std::string>* vecOutput)
		{
			std::string s;

			for (int c = 0; c < sInput.length(); c++)
			{
				switch (sInput[c])
				{
				case ',':
				{
					vecOutput->push_back(s);
					s.clear();
				}
				break;

				case ' ':
					break;

				default:
					s.push_back(sInput[c]);
				}
			}

		}

	public:
		void SetFileName(std::string sFileName)
		{
			m_sFileName = sFileName;
		}

		bool Load()
		{
			if (!m_ifCsvFile.is_open())
				m_ifCsvFile.open(m_sFileName, std::ios::binary);

			if (!m_ifCsvFile.is_open())
				return false;

			for (int i = 0; !m_ifCsvFile.eof(); i++)
			{
				if (m_ifCsvFile.bad())
					return false;

				std::string s;
				std::vector<std::string> vecOut;
				Cell c;

				std::getline(m_ifCsvFile, s);
				Split(s, &vecOut);

				for (int j = 0; j < vecOut.size(); j++)
				{
					c.nCol = j;
					c.nRow = i;
					c.sValue = vecOut[j];

					m_vecCells.push_back(c);
				}

				c.nCol++;
				c.sValue = '\n';

				m_vecCells.push_back(c);
			}

			m_vecCells.pop_back();
			m_vecCells.pop_back();

			m_ifCsvFile.close();

			return true;
		}

		std::string GetValue(int row, int col)
		{
			for (const auto cell : m_vecCells)
			{
				if (cell.nRow == row && cell.nCol == col)
					return cell.sValue;
			}
		}

		void Print()
		{
			std::cout << "1 ";

			for (const auto cell : m_vecCells)
			{
				if (cell.sValue == "\n")
					std::cout << "\n" << cell.nRow + 2 << " ";
				else
					std::cout << " " << cell.sValue;
			}
		}

		inline int GetTotalCols()
		{
			return m_vecCells[m_vecCells.size() - 1].nCol + 1;
		}

		inline int GetTotalRows()
		{
			return m_vecCells[m_vecCells.size() - 1].nRow + 1;
		}

	};
}
