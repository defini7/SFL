#define SFL_DATAFILE
#include "SFL.h"

int main()
{
	sfl::DataFile df;

	df["Info"]["Name"].SetString("John");
	df["Info"]["Age"].SetInt(20);
	df["Info"]["Level"].SetInt(100);
	df["Info"]["Alive"].SetBool(true);

	sfl::DataFile::Write(df, "hello.df");
	
	return 0;
}
