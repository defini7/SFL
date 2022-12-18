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

#include <string>
#include <list>
#include <chrono>
#include <thread>
#include <iostream>

#pragma endregion

namespace sfl
{
	struct SourceLoc
	{
		SourceLoc(const uint32_t nLine = __builtin_LINE(),
			const uint32_t nColumn = __builtin_COLUMN(), const char* const sFile = __builtin_FILE(),
			const char* const sFunction = __builtin_FUNCTION())
		{
			m_nLine = nLine;
			m_nColumn = nColumn;
			m_sFile = sFile;
			m_sFunction = sFunction;
		}

		constexpr uint_least32_t line() const { return m_nLine; }
		constexpr uint_least32_t column() const { return m_nColumn; }
		constexpr const char* file() const { return m_sFile; }
		constexpr const char* function() const { return m_sFunction; }

	private:
		uint32_t m_nLine;
		uint32_t m_nColumn;
		const char* m_sFile;
		const char* m_sFunction;
	};

	struct Test
	{
		bool (*fTest)(void);
		std::string sName;

		bool bIgnored;
		bool bMeasured;
		bool bShouldPanic;

		std::string sFailNote;
	};

	class Tester
	{
	public:
		Tester() = default;
		~Tester()
		{
			if (m_tTestThread.joinable())
				m_tTestThread.join();
		}

	private:
		std::list<Test> m_listTests;

		std::thread m_tTestThread;

		bool m_bAlsoRunIgnored = false;
		bool m_bOnlyRunIgnored = false;

		std::string m_sFilter;

	public:
		void AddTest(bool (*fTest)(void), std::string sName, bool bIgnored = false, bool bMeasured = false, bool bShouldPanic = false, std::string sFailNote = "")
		{
			Test t;

			t.fTest = fTest;
			t.sName = sName;
			t.bIgnored = bIgnored;
			t.bMeasured = bMeasured;
			t.bShouldPanic = bShouldPanic;
			t.sFailNote = sFailNote;

			m_listTests.push_back(t);
		}

		void StartTests(bool bAlsoRunIgnored = false, bool bOnlyRunIgnored = false, std::string sFilter = "")
		{
			m_bAlsoRunIgnored = bAlsoRunIgnored;
			m_bOnlyRunIgnored = bOnlyRunIgnored;

			m_sFilter = sFilter;

			m_tTestThread = std::thread(&Tester::TestThread, this);
		}

	private:
		void TestThread()
		{
			int i = 1;

			int nPassed = 0;
			int nFailed = 0;
			int nIgnored = 0;
			int nMeasured = 0;

			auto tpTimePoint1 = std::chrono::steady_clock::now();
			auto tpTimePoint2 = std::chrono::steady_clock::now();

			float fSeconds = 0.0f;

			auto tpStart = std::chrono::steady_clock::now();

			for (auto& test : m_listTests)
			{
				if (test.bIgnored && !m_bOnlyRunIgnored && !m_bAlsoRunIgnored)
					nIgnored++;
				else
				{
					if (((test.bIgnored && m_bAlsoRunIgnored) || (test.bIgnored && m_bOnlyRunIgnored)) || (!test.bIgnored && !m_bOnlyRunIgnored))
					{
						if (test.sName.find(m_sFilter) != std::string::npos)
						{
							std::cout << "running " << i << " test\n";

							if (test.bMeasured)
								tpTimePoint1 = std::chrono::steady_clock::now();

							bool bPassed = test.fTest();

							if (test.bShouldPanic)
							{
								if (bPassed)
								{
									bPassed = false;
									Assert(false, "test did not failed as expected");
								}
								else
								{
									bPassed = true;

									if (!test.sFailNote.empty())
										std::cerr << "fail note: " << test.sFailNote << '\n';
								}
							}

							if (test.bMeasured)
							{
								tpTimePoint2 = std::chrono::steady_clock::now();
								fSeconds = (tpTimePoint2.time_since_epoch() - tpTimePoint1.time_since_epoch()).count() / 1000000.0f;
							}

							std::cout << "test " << test.sName << " ... " << std::boolalpha << bPassed << '\n';

							if (test.bMeasured)
							{
								std::cout << "took " << fSeconds << "s\n\n";
								nMeasured++;
							}
							else
								std::cout << '\n';

							if (bPassed)
								nPassed++;
							else
								nFailed++;

							i++;
						}
					}
				}
			}

			float fSecondsTook;

			if (nPassed > 0 || nFailed > 0 || nIgnored > 0)
				fSecondsTook = std::chrono::duration<float>(std::chrono::steady_clock::now() - tpStart).count();
			else
				fSecondsTook = 0.0f;

			bool bTestResult = (nFailed == 0);

			std::cout << "test result: " << std::boolalpha << bTestResult << ". " <<
				nPassed << " passed; " << nFailed << " failed; " << nIgnored << " ignored; " <<
				nMeasured << " measured; finished in " << fSecondsTook << "s\n";
		}
	};

	bool Assert(bool bExpr, std::string sMsg = "undefined", SourceLoc sl = SourceLoc())
	{
		if (!bExpr)
		{
			std::cerr << "assertion failed with message: " << sMsg <<
				". In " << sl.function() <<
				", at " << sl.line() << ':' << sl.column() << '\n';
		}

		return bExpr;
	}
}
