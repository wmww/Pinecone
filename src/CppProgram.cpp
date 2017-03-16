#include "../h/CppProgram.h"
#include "../h/msclStringFuncs.h"

CppFuncBase::CppFuncBase(string prototypeIn)
{
	prototype=prototypeIn;
}

void CppFuncBase::code(string in)
{
	if (freshLine)
	{
		source+=indentString(in, indent, indentLevel);
		freshLine=(in.back()=='\n');
	}
	else if (searchInString(in, "\n")>0)
	{
		source+="\n"+indentString(in, indent, indentLevel);
		if (in.back()!='\n')
			source+="\n";
		freshLine=true;
	}
	else
	{
		source+=in;
		freshLine=(in.back()=='\n');
	}
}

void CppFuncBase::line(string in)
{
	code(in);
	endln();
}

void CppFuncBase::endln()
{
	if (exprLevel>0)
	{
		throw PineconeError("non zero expression level when ending line in C++ program", INTERNAL_ERROR);
	}
	else if (freshLine)
	{
		source=source.substr(0, source.size()-1) + ";\n";
	}
	else
	{
		source+=";\n";
	}
	
	freshLine=true;
}

void CppFuncBase::comment(string in)
{
	if (searchInString(in, "\n")>=0)
	{
		source+=indentString("\n/*\n"+in+"\n*/\n", indent, indentLevel);
		freshLine=true;
	}
	else if (exprLevel>0 || source.back()!='\n')
	{
		source+="/* "+in+" */";
		freshLine=false;
	}
	else
	{
		source+=indentString("// "+in+"\n", indent, indentLevel);
		freshLine=true;
	}
};

void CppFuncBase::pushExpr()
{
	code("(");
	exprLevel++;
	freshLine=false;
}

void CppFuncBase::popExpr()
{
	if (exprLevel<=0)
	{
		throw PineconeError("CppProgram::popExpression called with zero expressionLevel", INTERNAL_ERROR);
	}
	
	code(")");
	exprLevel--;
	freshLine=false;
}

void CppFuncBase::pushBlock()
{
	if (exprLevel>0)
	{
		throw PineconeError("CppProgram::pushBlock called when expressionLevel was not zero", INTERNAL_ERROR);
	}
	
	code("{\n");
	indentLevel++;
	freshLine=true;
}

void CppFuncBase::popBlock()
{
	if (indentLevel<=0)
	{
		throw PineconeError("CppProgram::popBlock called with zero indentationLevel", INTERNAL_ERROR);
	}
	
	indentLevel--;
	code("}\n\n");
	freshLine=true;
}
