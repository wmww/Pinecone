#include "../h/CppProgram.h"
#include "../h/msclStringFuncs.h"

void CppProgram::addCode(string in)
{
	code+=indentString(in+"\n", indent, indentationLevel);
}

void CppProgram::addComment(string in)
{
	if (searchInString(in, "\n") == -1)
	{
		code+=indentString("// " + in + "\n", indent, indentationLevel);
	}
	else
	{
		code+=indentString("/*\n"+in+"\n*/\n", indent, indentationLevel);
	}
};

void CppProgram::pushBlock()
{
	code+="{\n\n";
	indentationLevel++;
}

void CppProgram::popBlock()
{
	code+="}\n";
	indentationLevel--;
}
