#include "../h/CppProgram.h"
#include "../h/msclStringFuncs.h"

void CppProgram::addCode(string in)
{
	code+=indentString(in, indent);
}

void CppProgram::addComment(string in)
{
	code+=indentString("/* "+in+" */\n", indent);
};

void CppProgram::pushBlock()
{
	code+="{\n";
	indentationLevel++;
}

void CppProgram::popBlock()
{
	code+="}\n";
	indentationLevel--;
}
