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

#include <cmath>

#pragma endregion

namespace sfl
{
	template <class T>
	struct vec2d_basic
	{
		vec2d_basic() = default;
		vec2d_basic(T x, T y)
		{
			this->x = x;
			this->y = y;
		}

		T x;
		T y;

		friend vec2d_basic<T> operator+(const vec2d_basic<T>& v1, const vec2d_basic<T>& v2) { return { v1.x + v2.x, v1.y + v2.y }; }
		friend vec2d_basic<T> operator-(const vec2d_basic<T> v1, const vec2d_basic<T>& v2) { return { v1.x - v2.x, v1.y - v2.y }; }
		friend vec2d_basic<T> operator*(const vec2d_basic<T> v1, const vec2d_basic<T>& v2) { return { v1.x * v2.x, v1.y * v2.y }; }
		friend vec2d_basic<T> operator/(const vec2d_basic<T> v1, const vec2d_basic<T>& v2) { return { v1.x / v2.x, v1.y / v2.y }; }
		friend vec2d_basic<T> operator+(const vec2d_basic<T> v1, const T v) { return { v1.x + v, v1.y + v }; }
		friend vec2d_basic<T> operator-(const vec2d_basic<T> v1, const T v) { return { v1.x - v, v1.y - v }; }
		friend vec2d_basic<T> operator*(const vec2d_basic<T> v1, const T v) { return { v1.x * v, v1.y * v }; }
		friend vec2d_basic<T> operator/(vec2d_basic<T> v1, const T v) { return { v1.x / v,	v1.y / v }; }

		friend vec2d_basic<T> operator+=(vec2d_basic<T>& v1, const vec2d_basic<T>& v2)
		{
			v1.x += v2.x;
			v1.y += v2.y;
			return v1;
		}

		friend vec2d_basic<T> operator-=(vec2d_basic<T> v1, const vec2d_basic<T>& v2)
		{
			v1.x -= v2.x;
			v1.y -= v2.y;
			return v1;
		}

		friend vec2d_basic<T> operator*=(vec2d_basic<T> v1, const vec2d_basic<T>& v2)
		{
			v1.x *= v2.x;
			v1.y *= v2.y;
			return v1;
		}

		friend vec2d_basic<T> operator/=(vec2d_basic<T> v1, const vec2d_basic<T>& v2)
		{
			v1.x /= v2.x;
			v1.y /= v2.y;
			return v1;
		}

		friend vec2d_basic<T> operator+=(vec2d_basic<T> v1, const T v)
		{
			v1.x += v;
			v1.y += v;
			return v1;
		}

		friend vec2d_basic<T> operator-=(vec2d_basic<T> v1, const T v)
		{
			v1.x -= v;
			v1.y -= v;
			return v1;
		}

		friend vec2d_basic<T> operator*=(vec2d_basic<T> v1, const T v)
		{
			v1.x *= v;
			v1.y *= v;
			return v1;
		}

		friend vec2d_basic<T> operator/=(vec2d_basic<T> v1, const T v)
		{
			v1.x /= v;
			v1.y /= v;
			return v1;
		}

		friend bool operator<(const vec2d_basic<T> v1, const vec2d_basic<T> v) { return v1.x < v.x&& v1.y < v.y; }
		friend bool operator>(const vec2d_basic<T> v1, const vec2d_basic<T> v) { return v1.x > v.x && v1.y > v.y; }
		friend bool operator<=(const vec2d_basic<T> v1, const vec2d_basic<T> v) { return v1.x <= v.x && v1.y <= v.y; }
		friend bool operator>=(const vec2d_basic<T> v1, const vec2d_basic<T> v) { return v1.x >= v.x && v1.y >= v.y; }
		friend bool operator==(const vec2d_basic<T> v1, const vec2d_basic<T> v) { return v1.x == v.x && v1.y == v.y; }
		friend bool operator!=(const vec2d_basic<T> v1, const vec2d_basic<T> v) { return v1.x != v.x || v1.y != v.y; }

		friend bool operator<(const vec2d_basic<T> v1, const T v) { return v1.x < v&& v1.y < v; }
		friend bool operator>(const vec2d_basic<T> v1, const T v) { return v1.x > v && v1.y > v; }
		friend bool operator<=(const vec2d_basic<T> v1, const T v) { return v1.x <= v && v1.y <= v; }
		friend bool operator>=(const vec2d_basic<T> v1, const T v) { return v1.x >= v && v1.y >= v; }
		friend bool operator==(const vec2d_basic<T> v1, const T v) { return v1.x == v && v1.y == v; }
		friend bool operator!=(const vec2d_basic<T> v1, const T v) { return v1.x != v || v1.y != v; }

		template <typename T1>
		vec2d_basic<T1> to() { return vec2d_basic<T1>((T1)this->x, (T1)this->y); }

		float dot(vec2d_basic<T> v)
		{
			return this->x * v.x + this->y * v.y;
		}

		float length()
		{
			return sqrtf(dot(*this));
		}

		vec2d_basic<T> norm()
		{
			float l = length();
			return vec2d_basic<T>(this->x / l, this->y / l);
		}

		T mag() { return T(sqrtf(this->x * this->x + this->y * this->y)); }
		T mag2() { return T(this->x * this->x + this->y * this->y); }

		vec2d_basic<T> abs() { return vec2d_basic<T>(std::abs(this->x), std::abs(this->y)); }
		vec2d_basic<T> perp() { return vec2d_basic<T>(-this->y, this->x); }
		vec2d_basic<T> floor() { return vec2d_basic<T>(std::floor(this->x), std::floor(this->y)); }
		vec2d_basic<T> ceil() { return vec2d_basic<T>(std::ceil(this->x), std::ceil(this->y)); }
		vec2d_basic<T> cart() { return vec2d_basic<T>(cos(this->y) * x, sin(this->y) * this->x); }
		vec2d_basic<T> polar() { return vec2d_basic<T>(mag(), atan2(this->y, this->x)); }
		vec2d_basic<T>& ref() { return *this; }
	};

	typedef vec2d_basic<int> vi2d;
	typedef vec2d_basic<float> vf2d;
}
