#include "../h/CppProgram.h"
#include "../h/msclStringFuncs.h"

CppProgram::CppProgram()
{
	source="\n";
}

void CppProgram::code(string in)
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

void CppProgram::line(string in)
{
	code(in);
	endln();
}

void CppProgram::endln()
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

void CppProgram::comment(string in)
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

void CppProgram::pushExpr()
{
	code("(");
	exprLevel++;
	freshLine=false;
}

void CppProgram::popExpr()
{
	if (exprLevel<=0)
	{
		throw PineconeError("CppProgram::popExpression called with zero expressionLevel", INTERNAL_ERROR);
	}
	
	code(")");
	exprLevel--;
	freshLine=false;
}

void CppProgram::pushBlock()
{
	if (exprLevel>0)
	{
		throw PineconeError("CppProgram::pushBlock called when expressionLevel was not zero", INTERNAL_ERROR);
	}
	
	code("{\n");
	indentLevel++;
	freshLine=true;
}

void CppProgram::popBlock()
{
	if (indentLevel<=0)
	{
		throw PineconeError("CppProgram::popBlock called with zero indentationLevel", INTERNAL_ERROR);
	}
	
	indentLevel--;
	code("}\n\n");
	freshLine=true;
}
