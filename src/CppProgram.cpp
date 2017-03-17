#include "../h/CppProgram.h"
#include "../h/msclStringFuncs.h"

#include <map>
#include <unordered_set>


/// Name Container

CppNameContainer::CppNameContainer()
{
	
}

shared_ptr<CppNameContainer> CppNameContainer::makeRoot()
{
	auto out=shared_ptr<CppNameContainer>(new CppNameContainer());
	out->parent=nullptr;
	return out;
}

shared_ptr<CppNameContainer> CppNameContainer::makeChild()
{
	auto out=shared_ptr<CppNameContainer>(new CppNameContainer());
	children.push_back(out);
	out->parent=this;
	return out;
}

void CppNameContainer::addPn(const string& pn)
{
	if (pnToCppMap.find(pn)!=pnToCppMap.end())
	{
		throw PineconeError("Tried to add '"+pn+"' as a pn name to a CppNameContainer but that pn name already exists", INTERNAL_ERROR);
	}
	
	// now we need to find a unique C++ name, the pinecone name will almost always be unique, but there may be cases where Pinecone treats scope differently or uses a keyword or something
	
	string cpp=pn;
	int attempts=0;
	bool valid=!hasCpp(cpp);
	
	while (!valid )
	{
		string suffix;
		
		// I feel like this is a really bad way to do this, but I can't think of a better one
		
		if (attempts<10)
		{
			suffix=to_string(attempts); // this case should almost always happen
		}
		else if (attempts<120)
		{
			suffix=to_string(rand()%10000);
		}
		else
		{
			throw PineconeError("could not find unique name for '"+pn+"' in CppNameContainer::enterPn", INTERNAL_ERROR);
		}
		
		cpp=pn+"__"+suffix;
		attempts++;
		valid=!hasCpp(cpp);
	}
	
	pnToCppMap[pn]=cpp;
}

bool CppNameContainer::hasCpp(const string& cpp)
{
	return hasCppMe(cpp) || hasCppUp(cpp) || hasCppDown(cpp);
}

bool CppNameContainer::hasCppMe(const string& cpp)
{
	return cppSet.find(cpp)!=cppSet.end();
}

bool CppNameContainer::hasCppUp(const string& cpp)
{
	return parent && (parent->hasCppMe(cpp) || parent->hasCppUp(cpp));
}

bool CppNameContainer::hasCppDown(const string& cpp)
{
	for (auto i: children)
	{
		if (i->hasCppMe(cpp) || i->hasCppDown(cpp))
			return true;
	}
	
	return false;
}

string CppNameContainer::getCppForPn(const string& pn)
{
	auto result=pnToCppMap.find(pn);
	
	if (result==pnToCppMap.end())
	{
		if (parent)
			return parent->getCppForPn(pn);
		else
			throw PineconeError("could not find C++ equivalent of '"+pn+"' in CppNameContainer::getCppForPn", INTERNAL_ERROR);
	}
	else
		return result->second;
}


/// funcs

CppFuncBase::CppFuncBase(string prototypeIn, shared_ptr<CppNameContainer> globalNames)
{
	prototype=prototypeIn;
	namespaceStack.push_back(globalNames);
}

void CppFuncBase::code(const string& in)
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

void CppFuncBase::name(const string& in)
{
	comment("CppFuncBase::name not yet implemented");
}

void CppFuncBase::type(Type in)
{
	comment("CppFuncBase::type not yet implemented");
}

void CppFuncBase::line(const string& in)
{
	code(in);
	endln();
}

void CppFuncBase::declareVar(const string& nameIn, Type typeIn)
{
	namespaceStack.back()->addPn(nameIn);
	
	type(typeIn); code(" "); name(nameIn); endln();
}

void CppFuncBase::endln()
{
	if (exprLevel>0)
	{
		throw PineconeError("non zero expression level when ending line in C++ program", INTERNAL_ERROR);
	}
	else
	{
		source+=";\n";
	}
	
	freshLine=true;
}

void CppFuncBase::comment(const string& in)
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
	namespaceStack.push_back(namespaceStack.back()->makeChild());
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
	namespaceStack.pop_back();
	code("}\n\n");
	freshLine=true;
}


/// program

CppProgram::CppProgram()
{
	//funcs = unique_ptr<std::map<string, CppFunc>>(new std::map<string, CppFunc>());
	globalNames=CppNameContainer::makeRoot();
	pushFunc(string("main"), {}, Void);
}

bool CppProgram::hasFunc(const string& name)
{
	return funcs.find(name)!=funcs.end();
}

void CppProgram::pushFunc(const string& name, vector<NamedType> args, Type returnType)
{
	if (hasFunc(name))
	{
		throw PineconeError("called CppProgram::pushFunc with function name '"+name+"', which already exists", INTERNAL_ERROR);
	}
	
	globalNames->addPn(name);
	
	string cppName=globalNames->getCppForPn(name);
	
	string prototype;
	
	prototype+=returnType->getString();
	
	prototype+=" "+cppName+"(";
	
	for (int i=0; i<int(args.size()); i++)
	{
		if (i)
			prototype+=", ";
		
		prototype+=args[i].type->getString();
		
		prototype+=" ";
		
		prototype+="v_"+args[i].name;
	}
	
	if (!args.size())
	{
		prototype+="void";
	}
	
	prototype+=")";
	
	activeFunc=CppFunc(new CppFuncBase(prototype, globalNames));
	funcs[name]=activeFunc;
	funcStack.push_back(name);
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
