#include <iostream>

#include "Tester.h"

bool test1()
{
	return 1 + 1 == 2;
}

bool test2()
{
	return 5 * 5 == 25;
}

int main()
{
	Tester tester;

	tester.AddTest(test1, "test1", false, true);
	tester.AddTest(test2, "test2");

	tester.StartTests();

	return 0;
}
