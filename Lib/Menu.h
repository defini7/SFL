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

#include <unordered_map>
#include <algorithm>

#pragma endregion

namespace sfl
{
	namespace menu
	{
		struct Vec2
		{
			int32_t x;
			int32_t y;
		};

		enum ComponentState : uint32_t
		{
			Enabled,
			HasChildren
		};

		constexpr int32_t PATCH = 8;

		class Component
		{
		public:
			Component()
			{
				sName = "main";
			}

			Component(const std::string& name)
			{
				sName = name;
			}

			Component& SetTable(int32_t cols, int32_t rows)
			{
				vCellTable = { cols, rows };
				return *this;
			}

			Component& SetPadding(int32_t width, int32_t height)
			{
				vCellPad = { width, height };
				return *this;
			}

			Component& SetID(int32_t id)
			{
				nID = id;
				return *this;
			}

			Component& Enable(bool enable)
			{
				bEnabled = enable;
				return *this;
			}

			std::string& GetName()
			{
				return sName;
			}

			int32_t GetID()
			{
				return nID;
			}

			Vec2 GetSize()
			{
				return { int32_t(sName.size()), 1 };
			}

			Vec2 GetCursor()
			{
				return vCursorPos;
			}

			bool HasChildren()
			{
				return !mItems.empty();
			}

			bool Enabled()
			{
				return bEnabled;
			}

			Component& operator[](const std::string& name)
			{
				if (mItems.count(name) == 0)
				{
					mItems[name] = vecItems.size();
					vecItems.push_back(Component(name));
				}

				return vecItems[mItems[name]];
			}

			template <typename T1, typename T2>
			void DrawSelf(Vec2 vPos, T1 OnComponentDraw, T2 OnTextDraw)
			{
				Vec2 vSize;
				vSize.x = vSizeInPatches.x * PATCH;
				vSize.y = vSizeInPatches.y * PATCH;

				OnComponentDraw(vPos, vSize, PATCH);

				int32_t nTopLeft = nTopVisibleRow * vCellTable.x;
				int32_t nBottomRight = std::min((int32_t)vecItems.size(), vCellTable.y * vCellTable.x + nTopLeft);
				int32_t nVisible = nBottomRight - nTopLeft;

				Vec2 vCell = { 0, 0 };
				Vec2 vPatchPos = { 0, 0 };

				for (int32_t i = 0; i < nVisible; i++)
				{
					vCell.x = i % vCellTable.x;
					vCell.y = i / vCellTable.x;

					vPatchPos.x = vCell.x * (vCellSize.x + vCellPad.x) + 1;
					vPatchPos.y = vCell.y * (vCellSize.y + vCellPad.y) + 1;

					Vec2 vScreenLoc;
					vScreenLoc.x = vPos.x + vPatchPos.x * PATCH;
					vScreenLoc.y = vPos.y + vPatchPos.y * PATCH;

					bool bFlags[2];
					bFlags[ComponentState::Enabled] = vecItems[nTopLeft + i].bEnabled;
					bFlags[ComponentState::HasChildren] = (vecItems[nTopLeft + i].vecItems.size() > 0);

					OnTextDraw(vScreenLoc, vecItems[nTopLeft + i].sName, bFlags, PATCH);
				}

				vCursorPos.x = vPos.x + (vCellCursor.x * (vCellSize.x + vCellPad.x)) * PATCH - PATCH;
				vCursorPos.y = vPos.y + ((vCellCursor.y - nTopVisibleRow) * (vCellSize.y + vCellPad.y)) * PATCH + PATCH;
			}

			void Construct()
			{
				for (auto& item : vecItems)
				{
					if (item.HasChildren())
						item.Construct();

					vCellSize.x = std::max(item.GetSize().x, vCellSize.x);
					vCellSize.y = std::max(item.GetSize().y, vCellSize.y);
				}

				vSizeInPatches.x = vCellTable.x * vCellSize.x + (vCellTable.x - 1) * vCellPad.x + 2;
				vSizeInPatches.y = vCellTable.y * vCellSize.y + (vCellTable.y - 1) * vCellPad.y + 2;

				nTotalRows = (vecItems.size() / vCellTable.x) + (((vecItems.size() % vCellTable.x) > 0) ? 1 : 0);
			}

			void ClampCursor()
			{
				nCursorItem = vCellCursor.y * vCellTable.x + vCellCursor.x;

				if (nCursorItem >= (int32_t)vecItems.size())
				{
					vCellCursor.x = (vecItems.size() % vCellTable.x) - 1;
					vCellCursor.y = (vecItems.size() / vCellTable.x);

					nCursorItem = vecItems.size() - 1;
				}
			}

			void OnUp()
			{
				vCellCursor.y--;

				if (vCellCursor.y < 0)
					vCellCursor.y = 0;

				if (vCellCursor.y < nTopVisibleRow)
				{
					nTopVisibleRow--;

					if (nTopVisibleRow < 0)
						nTopVisibleRow = 0;
				}

				ClampCursor();
			}

			void OnDown()
			{
				vCellCursor.y++;

				if (vCellCursor.y == nTotalRows)
					vCellCursor.y = nTotalRows - 1;

				if (vCellCursor.y > nTopVisibleRow + vCellTable.y - 1)
				{
					nTopVisibleRow++;

					if (nTopVisibleRow > nTotalRows - vCellTable.y)
						nTopVisibleRow = nTotalRows - vCellTable.y;
				}

				ClampCursor();
			}

			void OnLeft()
			{
				vCellCursor.x--;

				if (vCellCursor.x < 0)
					vCellCursor.x = 0;

				ClampCursor();
			}

			void OnRight()
			{
				vCellCursor.x++;

				if (vCellCursor.x == vCellTable.x)
					vCellCursor.x = vCellTable.x - 1;

				ClampCursor();
			}

			Component* OnConfirm()
			{
				if (GetSelected()->HasChildren())
					return GetSelected();

				return this;
			}

			Component* GetSelected()
			{
				return &vecItems[nCursorItem];
			}

		protected:
			std::string sName;
			bool bEnabled = true;

			int32_t nID = -1;
			int32_t nTotalRows = 0;
			int32_t nTopVisibleRow = 0;

			Vec2 vCellTable = { 1, 0 };
			Vec2 vCellSize = { 0, 0 };
			Vec2 vCellPad = { 2, 0 };

			const Vec2 vPatchSize = { PATCH, PATCH };
			Vec2 vSizeInPatches = { 0, 0 };

			std::unordered_map<std::string, size_t> mItems;
			std::vector<Component> vecItems;

			Vec2 vCellCursor = { 0, 0 };
			int32_t nCursorItem = 0;
			Vec2 vCursorPos = { 0, 0 };
		};

		class Manager
		{
		public:
			Manager() = default;

			void Open(Component* mo)
			{
				Close();
				listObjects.push_back(mo);
			}

			void Close()
			{
				listObjects.clear();
			}

			void OnUp()
			{
				if (!listObjects.empty())
					listObjects.back()->OnUp();
			}

			void OnDown()
			{
				if (!listObjects.empty())
					listObjects.back()->OnDown();
			}

			void OnLeft()
			{
				if (!listObjects.empty())
					listObjects.back()->OnLeft();
			}

			void OnRight()
			{
				if (!listObjects.empty())
					listObjects.back()->OnRight();
			}

			void OnBack()
			{
				if (!listObjects.empty())
					listObjects.pop_back();
			}

			Component* OnConfirm()
			{
				if (listObjects.empty())
					return nullptr;

				Component* next = listObjects.back()->OnConfirm();

				if (next == listObjects.back())
				{
					if (listObjects.back()->GetSelected()->Enabled())
						return listObjects.back()->GetSelected();
				}
				else
				{
					if (next->Enabled())
						listObjects.push_back(next);
				}
			}

			template <typename T1, typename T2, typename T3>
			void DrawSelf(Vec2 vPos, T1 OnComponentDraw, T2 OnTextDraw, T3 OnCursorDraw)
			{
				if (listObjects.empty())
					return;

				for (auto& o : listObjects)
				{
					o->DrawSelf(vPos, OnComponentDraw, OnTextDraw);
					vPos.x += 10;
					vPos.y += 10;
				}

				Vec2 vCursorPos = listObjects.back()->GetCursor();
				vCursorPos.x += PATCH;

				Vec2 vCursorSize = { PATCH - 2, PATCH / 2 };

				OnCursorDraw(vCursorPos, vCursorSize);
			}

		private:
			std::list<Component*> listObjects;
		};
	}
}
