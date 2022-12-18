#define SFL_CONSOLE
#include "SFL.h"

int main()
{
	std::vector<char> vecStr = {
		'H', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd', '!'
	};

	std::string sText;
	sfl::ContToStr(vecStr.begin(), vecStr.end(), sText);

	sfl::Print(sfl::FG_RED + sfl::UNDERLINE + sfl::BOLD + sText + sfl::RESET_ALL);

	return 0;
}
