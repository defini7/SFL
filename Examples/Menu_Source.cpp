#include <iostream>

#define SFL_MENU
#include "SFL.h"

int main()
{
	sfl::menu::Component gc;
	sfl::menu::Manager gm;

	gc["main"].SetTable(3, 3).SetPadding(2, 1);
	gc["main"]["Reset"].SetID(10002);
	gc["main"]["Apply"].SetID(10000);

	gc["main"]["Remove"].SetTable(1, 3).SetID(10001);

	gc["main"]["Remove"]["First"].SetTable(1, 3).SetID(0);
	gc["main"]["Remove"]["First"]["Other1"].SetID(0);
	gc["main"]["Remove"]["First"]["Other2"].SetID(1);
	gc["main"]["Remove"]["First"]["Other3"].SetID(2);

	gc["main"]["Remove"]["Second"].SetID(1);
	gc["main"]["Remove"]["Third"].SetID(2);

	gc["main"]["Dummy1"].SetID(10002);
	gc["main"]["Dummy2"].SetID(10003);
	gc["main"]["Dummy3"].SetID(10004);
	gc["main"]["Dummy4"].SetID(10005).Enable(false);
	gc["main"]["Dummy5"].SetID(10006).Enable(false);
	gc["main"]["Dummy6"].SetID(10007).Enable(false);
	gc["main"]["Dummy7"].SetID(10008);

	gc.Construct();

	return 0;
}
