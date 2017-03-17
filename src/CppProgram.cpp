#include "../h/CppProgram.h"
#include "../h/msclStringFuncs.h"

#include <map>
#include <unordered_set>

string getValidCppId(string in)
{
	string cpp;
	
	int start=0;
	
	// all this should work with unicode (although it may inject random (but valid) characters into the C++ names)
	do
	{
		int i=start;
		
		for (; i<int(in.size()) && (
				(in[i]>='a' && in[i]<='z') ||
				(in[i]>='A' && in[i]<='Z') ||
				(in[i]>='0' && in[i]<='9') ||
				in[i]=='_'
			); i++) {}
		
		if (i!=start)
		{
			if (in[start]>='0' && in[start]<='9')
				cpp+="_";
			cpp+=in.substr(start, i-start);
		}
		
		start=i+1;
		
	} while (start<int(in.size()));
	
	if (cpp.empty())
		cpp="no_name";
	
	return cpp;
}


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
	
	string cpp=getValidCppId(pn); // strip illegal chars
	
	int attempts=0;
	bool valid=!hasCpp(cpp);
	
	while (!valid)
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

bool CppNameContainer::hasPn(const string& pn)
{
	return pnToCppMap.find(pn)!=pnToCppMap.end();
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
	code(namespaceStack.back()->getCppForPn(in));
}

void CppFuncBase::line(const string& in)
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

string CppProgram::getTypeCode(Type in)
{
	switch (in->getType())
	{
case TypeBase::VOID:
		return "void";
		
	case TypeBase::DUB:
		return "double";
		
	case TypeBase::INT:
		return "int";
		
	case TypeBase::BOOL:
		return "bool";
		
	case TypeBase::PTR:
		return "*"+getTypeCode(in->getSubType());
		
	case TypeBase::TUPLE:
	{
		string compact="{"+in->getCompactString()+"}";
		
		if (!globalNames->hasPn(compact))
		{
			globalNames->addPn(compact);
			
			string code;
			code+="struct ";
			code+=globalNames->getCppForPn(compact);
			code+="\n{\n";
			code+=indentString("int abc\n", indent);
			code+=indentString("double xyz\n", indent);
			code+="}\n";
			
			globalCode+=code;
		}
		
		return globalNames->getCppForPn(compact);
	}
	
	default:
		throw PineconeError("CppProgram::getTypeCode called with invalid type "+(TypeBase::getString(in->getType())), INTERNAL_ERROR);
	}
}

void CppProgram::declareVar(const string& nameIn, Type typeIn)
{
	activeFunc->namespaceStack.back()->addPn(nameIn);
	
	code(getTypeCode(typeIn)); code(" "); name(nameIn); endln();
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
	
	prototype+=getTypeCode(returnType);
	
	prototype+=" "+cppName+"(";
	
	for (int i=0; i<int(args.size()); i++)
	{
		if (i)
			prototype+=", ";
		
		prototype+=getTypeCode(args[i].type);
		
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
	
	out+=globalCode+"\n";
	
	for (auto i: funcs)
	{
		if (i.first!="main")
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


