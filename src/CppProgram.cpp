#include "../h/CppProgram.h"
#include "../h/msclStringFuncs.h"

void CppProgram::addCode(string in)
{
	if (code.back()=='\n')
	{
		code+=indentString(in, indent, indentationLevel);
	}
	else if (searchInString(in, "\n")>0)
	{
		code+="\n"+indentString(in, indent, indentationLevel);
		if (in.back()!='\n')
			code+="\n";
	}
	else
	{
		code+=in;
	}
}

void CppProgram::addComment(string in)
{
	if (searchInString(in, "\n")>=0)
	{
		code+=indentString("\n/*\n"+in+"\n*/\n", indent, indentationLevel);
	}
	else if (expressionLevel>0)
	{
		code+="/* "+in+" */";
	}
	else
	{
		code+=indentString("// "+in+"\n", indent, indentationLevel);
	}
};

void CppProgram::pushExpression()
{
	addCode("(");
	expressionLevel++;
}

void CppProgram::popExpression()
{
	if (expressionLevel<=0)
	{
		throw PineconeError("CppProgram::popExpression called with zero expressionLevel", INTERNAL_ERROR);
	}
	
	addCode(")");
	expressionLevel--;
}

void CppProgram::pushBlock()
{
	if (expressionLevel>0)
	{
		throw PineconeError("CppProgram::pushBlock called when expressionLevel was not zero", INTERNAL_ERROR);
	}
	
	addCode("{\n");
	indentationLevel++;
}

void CppProgram::popBlock()
{
	if (indentationLevel<=0)
	{
		throw PineconeError("CppProgram::popBlock called with zero indentationLevel", INTERNAL_ERROR);
	}
	
	indentationLevel--;
	addCode("}\n\n");
}
