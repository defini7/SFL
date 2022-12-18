#define SFL_FLAG
#include "SFL.h"

int main(int argc, const char** argv)
{
	sfl::Flag flag;

	sfl::Err err = flag.Parse(argc, argv);
	if (!err) std::cout << err.msg << std::endl;

	bool bPrintHelp = flag.Bool("help", false, "prints help");
	int64_t nOpLevel = flag.Int("O", 1, "optimization level");
	std::string sName = flag.Str("std", "c++14", "c++ version");

	std::cout << nOpLevel << std::endl;
	std::cout << sName << std::endl;

	if (bPrintHelp)
		flag.Help();
	
	return 0;
}
