#define SFL_BUILDER
#include "SFL.h"

int main()
{
	sfl::Builder builder;

	builder.SetCompilerExecCommand(sfl::Builder::GPP);
	builder.AddParam("hello.cpp");
	builder.AddParam("-o");
	builder.AddParam("hello");
	
	builder.Build();

	return 0;
}
