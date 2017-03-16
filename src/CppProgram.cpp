#include "../h/CppProgram.h"
#include "../h/msclStringFuncs.h"


/// funcs

CppFuncBase::CppFuncBase(string prototypeIn)
{
	prototype=prototypeIn;
}

void CppFuncBase::code(string in)
{
	if (freshLine)
	{
		source+=indentString(in, indent, blockLevel);
		freshLine=(in.back()=='\n');
	}
	else if (searchInString(in, "\n")>0)
	{
		source+="\n"+indentString(in, indent, blockLevel);
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
		source+=indentString("\n/*\n"+in+"\n*/\n", indent, blockLevel);
		freshLine=true;
	}
	else if (exprLevel>0 || source.back()!='\n')
	{
		source+="/* "+in+" */";
		freshLine=false;
	}
	else
	{
		source+=indentString("// "+in+"\n", indent, blockLevel);
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
	blockLevel++;
	freshLine=true;
}

void CppFuncBase::popBlock()
{
	if (blockLevel<=0)
	{
		throw PineconeError("CppProgram::popBlock called with zero indentationLevel", INTERNAL_ERROR);
	}
	
	blockLevel--;
	code("}\n\n");
	freshLine=true;
}


/// program

CppProgram::CppProgram()
{
	pushFunc("main", {}, Void);
}

bool CppProgram::getIfFuncExists(string name)
{
	return funcs.find(name)!=funcs.end();
}

void CppProgram::pushFunc(string name, vector<NamedType> args, Type returnType)
{
	if (getIfFuncExists(name))
	{
		throw PineconeError("called CppProgram::pushFunc with function name '"+name+"', which already exists", INTERNAL_ERROR);
	}
	
	string prototype;
	
	prototype+=getTypeName(returnType);
	
	prototype+=" "+name+"(";
	
	for (int i=0; i<int(args.size()); i++)
	{
		if (i)
			prototype+=", ";
		
		prototype+=getTypeName(args[i].type);
		
		prototype+=" ";
		
		prototype+="v_"+args[i].name;
	}
	
	if (!args.size())
	{
		prototype+="void";
	}
	
	prototype+=")";
	
	activeFunc=CppFunc(new CppFuncBase(prototype));
	funcs[name]=activeFunc;
}

void CppProgram::popFunc()
{
	if (activeFunc->getExprLevel()>0 || !activeFunc->getIfFreshLine() || activeFunc->getBlockLevel()>0)
	{
		throw PineconeError("called CppProgram::popFunc when function wasn't ready", INTERNAL_ERROR);
	}
	
	funcStack.pop_back();
	
	if (funcStack.empty())
	{
		throw PineconeError("called CppProgram::popFunc too many times", INTERNAL_ERROR);
	}
	
	activeFunc=funcs[funcStack.back()];
}

string CppProgram::getCppCode()
{
	string out;
	
	for (auto i: funcs)
	{
		out+=i.second->getPrototype()+";\n";
	}
	
	out+="\n";
	
	for (auto i: funcs)
	{
		out+=i.second->getPrototype()+"\n{\n";
		out+=indentString(i.second->getSource(), indent);
		out+="}\n\n";
	}
	
	return out;
}
