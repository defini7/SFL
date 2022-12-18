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

#include <cstdlib>
#include <cstdint>
#include <string>
#include <fstream>
#include <vector>

#pragma warning(disable : 4996)

#pragma endregion

namespace sfl
{
	namespace bmp
	{
		struct FileHeader
		{
			uint16_t  nType;
			uint32_t  nSize;
			uint16_t  nReserved1;
			uint16_t  nReserved2;
			uint32_t  nOffBits;
		};

		struct InfoHeader
		{
			uint32_t  nSize;
			int32_t   nWidth;
			int32_t   nHeight;
			uint16_t  nPlanes;
			uint16_t  nBitCount;
			uint32_t  nCompression;
			uint32_t  nSizeImage;
			int32_t   nXPelsPerMeter;
			int32_t   nYPelsPerMeter;
			uint32_t  nClrUsed;
			uint32_t  nClrImportant;
		};

		struct Rgb
		{
			uint8_t r;
			uint8_t g;
			uint8_t b;
		};

		struct Bmp
		{
			uint32_t nWidth = 0;
			uint32_t nHeight = 0;

			std::vector<Rgb> vecPixels;
		};

		enum ErrCode : uint32_t
		{
			Ok,
			InvalidFile,
		};

		bool IsValidBmp(FileHeader& fh, InfoHeader& ih)
		{
			return fh.nType == 0x4D42 && fh.nOffBits == 54 && ih.nSize == 40 &&
				ih.nBitCount == 24 && ih.nCompression == 0;
		}

		uint32_t MakePadding(uint32_t nWidth)
		{
			return (4 - (nWidth * sizeof(Rgb)) % 4) % 4;
		}

		void ReadFileHeader(std::fstream& fs, FileHeader& fh)
		{
			fs.read((char*)&fh.nType, sizeof(uint16_t));
			fs.read((char*)&fh.nSize, sizeof(uint32_t));
			fs.read((char*)&fh.nReserved1, sizeof(uint16_t));
			fs.read((char*)&fh.nReserved2, sizeof(uint16_t));
			fs.read((char*)&fh.nOffBits, sizeof(uint32_t));
		}

		void ReadInfoHeader(std::fstream& fs, InfoHeader& ih)
		{
			fs.read((char*)&ih.nSize, sizeof(uint32_t));
			fs.read((char*)&ih.nWidth, sizeof(int32_t));
			fs.read((char*)&ih.nHeight, sizeof(int32_t));
			fs.read((char*)&ih.nPlanes, sizeof(uint16_t));
			fs.read((char*)&ih.nBitCount, sizeof(uint16_t));
			fs.read((char*)&ih.nCompression, sizeof(uint32_t));
			fs.read((char*)&ih.nSizeImage, sizeof(uint32_t));
			fs.read((char*)&ih.nXPelsPerMeter, sizeof(int32_t));
			fs.read((char*)&ih.nYPelsPerMeter, sizeof(int32_t));
			fs.read((char*)&ih.nClrUsed, sizeof(uint32_t));
			fs.read((char*)&ih.nClrImportant, sizeof(uint32_t));
		}

		ErrCode Load(Bmp& bmp, std::string sFileName)
		{
			std::fstream fImage(sFileName, std::ios::binary | std::ios::in);

			FileHeader fh;
			ReadFileHeader(fImage, fh);

			InfoHeader ih;
			ReadInfoHeader(fImage, ih);

			if (!IsValidBmp(fh, ih))
			{
				fImage.close();
				return ErrCode::InvalidFile;
			}

			bmp.nWidth = ih.nWidth;
			bmp.nHeight = abs(ih.nHeight);

			bmp.vecPixels.resize(bmp.nWidth * bmp.nHeight);

			uint32_t nPadding = MakePadding(bmp.nWidth);

			for (uint32_t y = 0; y < bmp.nHeight; y++)
			{
				uint32_t nOffset = y * bmp.nWidth;
				for (uint32_t x = 0; x < bmp.nWidth; x++)
				{
					size_t i = nOffset + x;

					uint8_t rgb[3];
					fImage.read((char*)&rgb, sizeof(rgb));

					bmp.vecPixels[i].r = rgb[2];
					bmp.vecPixels[i].g = rgb[1];
					bmp.vecPixels[i].b = rgb[0];
				}

				fImage.seekg(nPadding, std::ios::cur);
			}

			fImage.close();

			return ErrCode::Ok;
		}
	}
}
