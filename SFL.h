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

#ifdef SFL_BUILDER
#include "Lib/Builder.h"
#endif

#ifdef SFL_FLAG
#include "Lib/Flag.h"
#endif

#ifdef SFL_TESTER
#include "Lib/Tester.h"
#endif

#ifdef SFL_DATAFILE
#include "Lib/DataFile.h"
#endif

#ifdef SFL_NET
#include "Lib/Net/Net.h"
#endif

#ifdef SFL_CSVREADER
#include "Lib/CsvReader.h"
#endif

#ifdef SFL_VEC
#include "Lib/Vec.h"
#endif

#ifdef SFL_BMP
#include "Lib/Bmp.h"
#endif

#ifdef SFL_CONSOLE
#include "Lib/Console.h"
#endif

#ifdef SFL_FIXEDPOINT
#include "Lib/FixedPoint.h"
#endif

#ifdef SFL_MENU
#include "Lib/Menu.h"
#endif
