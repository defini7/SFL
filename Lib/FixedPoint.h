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

#include <intrin.h>

#pragma endregion

namespace def
{
	template <size_t dp>
	struct fixed
	{
		constexpr fixed(double d)
		{
			// int(d * double(1 << dp) + (d >= 0 ? 0.5 : -0.5));

			__m256d _d, _zero, _m, _mask, _half;

			_half = _mm256_set1_pd(0.5);

			// double(1 << dp)
			_sh = _mm256_set1_pd(double(1 << dp));

			_d = _mm256_set1_pd(d);
			_zero = _mm256_set1_pd(0.0);

			// d * double(1 << dp)
			_m = _mm256_mul_pd(_d, _sh);

			_mask = _mm256_cmp_pd(_d, _zero, _CMP_GE_OQ);

			if (_mm256_movemask_pd(_mask) > 0)
				_m = _mm256_add_pd(_m, _half);
			else
				_m = _mm256_sub_pd(_m, _half);

			_v = _mm256_castpd_si256(_m);
		}

		constexpr double as_double() const
		{
			__m256d _r, _d_v;

			// (double)v / (double)(1 << dp);

			_d_v = _mm256_castsi256_pd(_v);

			_r = _mm256_div_pd(_d_v, _sh);

	#ifdef _WIN32
			return _r.m256d_f64[0];
	#else
			return _r[0];
	#endif
		}

		constexpr int as_fixed() const
		{
	#ifdef _WIN32
			return (int)_v.m256i_i32[0];
	#else
			return (int)_v[0];
	#endif
		}

	private:
		__m256i _v;
		__m256d _sh;

	};

	typedef fixed<16> fp16_t;
	typedef fixed<10> fp10_t;
	typedef fixed<8> fp8_t;
	typedef fixed<4> fp4_t;
}
