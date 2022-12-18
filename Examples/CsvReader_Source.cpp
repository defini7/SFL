#define SFL_CSVREADER
#include "SFL.h"

int main()
{
	sfl::CsvReader reader;
	reader.SetFileName("dna.csv");
	
	if (!reader.Load())
		return 1;

	reader.Print();
	
	return 0;
}
