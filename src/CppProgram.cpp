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

char getRandChar()
{
	static unsigned int seed = 1;
	//seed = (unsigned)newseed & 0x7fffffffU;
	
	seed = (seed * 1103515245U + 12345U) & 0x7fffffffU;
	
	int num=seed%(26+26+10);
	
	if (num<26)
	{
		return num+'a';
	}
	else if (num<26+26)
	{
		return num-26+'A';
	}
	else if (num<26+26+10)
	{
		return num-26-26+'0';
	}
	
	return '_';
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

void CppNameContainer::addPn(const string& pn, const string& cppNameHint)
{
	string validCppHint;
	
	if (cppNameHint=="<- the value of that pn string please")
		validCppHint=getValidCppId(pn);
	else if (!cppNameHint.empty())
		validCppHint=getValidCppId(cppNameHint);
	
	if (pnToCppMap.find(pn)!=pnToCppMap.end())
	{
		throw PineconeError("Tried to add '"+pn+"' as a pn name to a CppNameContainer but that pn name already exists", INTERNAL_ERROR);
	}
	
	// now we need to find a unique C++ name, the pinecone name will almost always be unique, but there may be cases where Pinecone treats scope differently or uses a keyword or something
	
	string cpp;
	if (!validCppHint.empty())
		cpp=validCppHint; // strip illegal chars
	
	int attempts=0;
	bool invalid=validCppHint.empty() || hasCpp(cpp);
	
	while (invalid)
	{
		// I feel like this is a really bad way to do this, but I can't think of a better one
		
		if (validCppHint.empty() || attempts>=100)
		{
			cpp=
				validCppHint.empty() ?
					"nm_"
				:
					validCppHint+"_";
			
			if (attempts<8)
			{
				for (int i=0; i<6; i++)
					cpp+=getRandChar();
			}
			else if (attempts<120)
			{
				for (int i=0; i<12; i++)
					cpp+=getRandChar();
			}
			else
			{
				throw PineconeError("could not find unique random name in CppNameContainer::enterPn", INTERNAL_ERROR);
			}
		}
		else
		{
			string suffix;
			
			if (attempts<100)
			{
				suffix=to_string(attempts); // this case should almost always happen
			}
			
			cpp=validCppHint+"_"+suffix;
		}
		attempts++;
		invalid=hasCpp(cpp);
	}
	
	pnToCppMap[pn]=cpp;
	cppSet.insert(cpp);
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

string CppNameContainer::getCpp(const string& pn)
{
	auto result=pnToCppMap.find(pn);
	
	if (result==pnToCppMap.end())
	{
		if (parent)
			return parent->getCpp(pn);
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
	code(namespaceStack.back()->getCpp(in));
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
	else if (freshLine && source[source.size()-2]=='}')
	{
		// do nothing
		
		//source+=indentString("\n", indent, blockLevel);
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
	code("}\n");
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
			globalNames->addPn(compact, in->nameHint);
			auto names=globalNames->makeChild();
			string code;
			code+="struct ";
			code+=globalNames->getCpp(compact);
			code+="\n{\n";
			for (auto i: *in->getAllSubTypes())
			{
				names->addPn(i.name);
				code+=indentString(getTypeCode(i.type)+" "+names->getCpp(i.name)+";\n", indent);
			}
			code+="\n";
			auto conNames=names->makeChild();
			code+=indentString(globalNames->getCpp(compact), indent);
			code+="(";
			bool first=true;
			for (auto i: *in->getAllSubTypes())
			{
				if (first) first=false; else code+=", ";
				conNames->addPn("+"+i.name+"_in");
				code+=getTypeCode(i.type)+" "+conNames->getCpp("+"+i.name+"_in");
			}
			code+=")\n";
			code+=indentString("{\n", indent);
			for (auto i: *in->getAllSubTypes())
			{
				code+=indentString(conNames->getCpp(i.name)+" = "+conNames->getCpp("+"+i.name+"_in")+";\n", indent, 2);
			}
			code+=indentString("}\n", indent);
			
			code+="};\n\n";
			
			globalCode+=code;
		}
		
		return globalNames->getCpp(compact);
	}
	
	default:
		throw PineconeError("CppProgram::getTypeCode called with invalid type "+(TypeBase::getString(in->getType())), INTERNAL_ERROR);
	}
}

void CppProgram::declareVar(const string& nameIn, Type typeIn)
{
	activeFunc->namespaceStack.back()->addPn(nameIn, nameIn);
	
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
	
	globalNames->addPn(name, name);
	
	string cppName=globalNames->getCpp(name);
	
	string prototype;
	
	prototype+=getTypeCode(returnType);
	
	prototype+=" "+cppName+"(";
	
	for (int i=0; i<int(args.size()); i++)
	{
		if (i)
			prototype+=", ";
		
		prototype+=getTypeCode(args[i].type);
		
		prototype+=" ";
		
		globalNames->addPn(args[i].name, args[i].name);
		
		prototype+=globalNames->getCpp(args[i].name);
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


