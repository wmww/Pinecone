#include "../h/CppProgram.h"
#include "../h/msclStringFuncs.h"

#include <map>
#include <unordered_set>

string getValidCppId(string in)
{
	string cpp;
	
	if (in.empty())
		in="no_name";
	
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
	
	if (validCppHint.empty())
	{
		cpp=getUniqueString(
				"nm",
				[this](string in) -> bool
				{
					return !hasCpp(in);
				},
				true
			);
	}
	else
	{
		cpp=getUniqueString(
				validCppHint,
				[this](string in) -> bool
				{
					return !hasCpp(in);
				},
				false
			);
	}
	
	pnToCppMap[pn]=cpp;
	cppSet.insert(cpp);
}

void CppNameContainer::reserveCpp(const string& cpp, bool ignoreCollisions)
{
	if (!ignoreCollisions && hasCpp(cpp))
	{
		throw PineconeError("called CppNameContainer::reserveCpp with id '"+cpp+"', which already exists", INTERNAL_ERROR);
	}
	
	cppSet.insert(cpp);
}

bool CppNameContainer::hasPnMe(const string& pn)
{
	return pnToCppMap.find(pn)!=pnToCppMap.end();
}

bool CppNameContainer::hasPn(const string& pn)
{
	return hasPnMe(pn) || (parent && parent->hasPn(pn));
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

CppFuncBase::CppFuncBase(string prototypeIn, shared_ptr<CppNameContainer> myNames, bool returnsValIn)
{
	prototype=prototypeIn;
	namespaceStack.push_back(myNames);
	freshLine=true;
	returnsVal=returnsValIn;
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
		throw PineconeError("non zero expression level when ending line in C++ program, code so far:\n"+indentString(source), INTERNAL_ERROR);
	}
	else if (freshLine && (source.size()<2 || source[source.size()-2]==';' || source[source.size()-2]=='}' || source[source.size()-2]=='{'))
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
	else if (exprLevel>0 || !freshLine)
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
	
	{
		code("{\n");
		namespaceStack.push_back(namespaceStack.back()->makeChild());
		blockLevel++;
		freshLine=true;
	}
}

void CppFuncBase::popBlock()
{
	
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
}

string CppFuncBase::pnToCpp(const string& in)
{
	return namespaceStack.back()->getCpp(in);
}


/// program

CppProgram::CppProgram()
{
	//funcs = unique_ptr<std::map<string, CppFunc>>(new std::map<string, CppFunc>());
	globalNames=CppNameContainer::makeRoot();
	setup();
}

void CppProgram::setup()
{
	globalTopCode+="// this C++ code is transpiled from Pinecone\n";
	globalTopCode+="// Pinecone v"+to_string(VERSION_X)+"."+to_string(VERSION_Y)+"."+to_string(VERSION_Z)+" was used\n";
	globalIncludesCode+="#include <string.h>\n";
	globalIncludesCode+="#include <stdlib.h>\n";
	globalIncludesCode+="#include <stdio.h>\n";
	
	globalVarCode+="int argc = 0;\n";
	globalVarCode+="char** argv = 0;\n";
	
	vector<string> cppReservedWords
	{
		// from C
		"auto", "const", "double", "float", "int", "short", "struct", "unsigned", "break",
		"continue", "else", "for", "long", "signed", "switch", "void", "case", "default",
		"enum", "goto", "register", "sizeof", "typedef", "volatile", "char", "do", "extern",
		"if", "return", "static", "union", "while",
		
		// from old C++
		"asm", "dynamic_cast", "namespace", "reinterpret_cast", "try", "bool", "explicit",
		"new", "static_cast", "typeid", "catch", "false", "operator", "template", "typename",
		"class", "friend", "private", "this", "using", "const_cast", "inline", "public",
		"throw", "virtual", "delete", "mutable", "protected", "true", "wchar_t", 
		
		// from C++11
		"and", "bitand", "compl", "not_eq", "or_eq", "xor_eq", "and_eq", "bitor", "not", "or",
		"xor",
		
		// something else
		"endl", "INT_MIN", "std", "INT_MAX", "MAX_RAND", "NULL",
		
		// my custom
		"main", "argc", "argv",
	};
	/*
	{
		"and", "and_eq", "asm", "auto", "bitand", "bitor", "bool", "break", "case", "catch", "char", "class", "const", "const_cast",
		"continue", "default", "delete", "do", "double", "dynamic_cast", "else", "enum", "explicit", "export", "extern", "false", "float",
		"for", "friend", "goto", "if", "inline", "int", "long", "mutable", "namespace", "new", "not", "not_eq", "operator", "or", "or_eq",
		"private", "protected", "public", "register", "reinterpret_cast", "return", "short", "signed", "sizeof", "static", "static_cast",
		"struct", "switch", "template", "this", "throw", "true", "try", "typedef", "typeid", "typename", "union", "unsigned", "using",
		"virtual", "void", "volatile", "wchar_t", "while", "xor", "xor_eq", ""
	};
	*/
		
	for (auto i: cppReservedWords)
	{
		globalNames->reserveCpp(i);
	}
	
	pushFunc(string("_main"), Void, Void, Void);
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
	
	case TypeBase::BYTE:
		return "unsigned char";
	
	case TypeBase::BOOL:
		return "bool";
		
	case TypeBase::PTR:
		if (in->getSubType()->isWhatev())
			return "void *";
		else
			return getTypeCode(in->getSubType())+" *";
		
	case TypeBase::TUPLE:
	{
		if (in->getAllSubTypes()->size()==1)
			return getTypeCode((*in->getAllSubTypes())[0].type);
		
		string compact="{"+in->getCompactString()+"}";
		
		if (!globalNames->hasPn(compact))
		{
			globalNames->addPn(compact, in->nameHint);
			string code;
			code+="struct ";
			code+=globalNames->getCpp(compact);
			code+="\n{\n";
			for (auto i: *in->getAllSubTypes())
			{
				code+=indentString(getTypeCode(i.type)+" "+i.name+";\n", indent);
			}
			
			code+="\n";
			code+=indentString(globalNames->getCpp(compact), indent);
			code+="() {}\n";
			
			code+="\n";
			auto conNames=globalNames->makeChild();
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
				code+=indentString(i.name+" = "+conNames->getCpp("+"+i.name+"_in")+";\n", indent, 2);
			}
			code+=indentString("}\n", indent);
			
			code+="};\n";
			
			if (!globalTypesCode.empty())
				globalTypesCode+="\n";
			globalTypesCode+=code;
		}
		
		return globalNames->getCpp(compact);
	}
	
	default:
		throw PineconeError("CppProgram::getTypeCode called with invalid type "+(TypeBase::getString(in->getType())), INTERNAL_ERROR);
	}
}

void CppProgram::declareVar(const string& nameIn, Type typeIn, string initialValue)
{
	/*
	CppNameContainer* names=&*activeFunc->namespaceStack.back();
	
	while (names)
	{
		if (names->hasPnMe(nameIn))
			return;
		
		names=names->getParent();
	}
	*/
	
	if (isMain())
	{
		declareGlobal(nameIn, typeIn, initialValue);
		return;
	}
	
	if (activeFunc->namespaceStack.back()->hasPn(nameIn))
	{
		return;
	}
	
	activeFunc->namespaceStack.back()->addPn(nameIn);
	activeFunc->namespaceStack[0]->reserveCpp(pnToCpp(nameIn), true);
	
	activeFunc->varDeclareSource+=getTypeCode(typeIn)+" "+pnToCpp(nameIn);
	if (!initialValue.empty())
	{
		activeFunc->varDeclareSource+=" = "+initialValue;
	}
	activeFunc->varDeclareSource+=";\n";
}

void CppProgram::declareGlobal(const string& nameIn, Type typeIn, string initialValue)
{
	if (globalNames->hasPn(nameIn))
		return;
	
	string code;
	code+=getTypeCode(typeIn);
	code+=" ";
	globalNames->addPn(nameIn);
	code+=globalNames->getCpp(nameIn);
	if (!initialValue.empty())
	{
		code+=" = ";
		code+=initialValue;
	}
	code+=";\n";
	
	globalVarCode+=code;
}

void CppProgram::addHeadCode(const string& code)
{
	globalIncludesCode += code + "\n";
}

bool CppProgram::hasFunc(const string& name)
{
	return funcs.find(name)!=funcs.end();
}

void CppProgram::addFunc(const string& name, vector<std::pair<string, string>> args, string returnType, string contents)
{
	if (hasFunc(name))
	{
		throw PineconeError("called CppProgram::addFunc with function name '"+name+"', which already exists", INTERNAL_ERROR);
	}
	
	globalNames->addPn(name, name);
	
	string cppName=globalNames->getCpp(name);
	
	string prototype;
	
	prototype+=returnType;
	
	prototype+=" "+cppName+"(";
	
	for (int i=0; i<int(args.size()); i++)
	{
		if (i)
			prototype+=", ";
		
		prototype+=args[i].first;
		
		prototype+=" ";
		
		prototype+=args[i].second;
	}
	
	prototype+=")";
	
	auto func=CppFunc(new CppFuncBase(prototype, globalNames->makeChild(), (returnType!="" && returnType!="void")));
	funcs[name]=func;
	func->code(contents);
}

//void CppProgram::pushFunc(const string& name, vector<std::pair<string, string>> args, Type returnType)
void CppProgram::pushFunc(const string& name, const string& cppNameHint, Type leftIn, Type rightIn, Type returnType)
{
	if (hasFunc(name))
	{
		throw PineconeError("called CppProgram::pushFunc with function name '"+name+"', which already exists", INTERNAL_ERROR);
	}
	
	globalNames->addPn(name, cppNameHint);
	
	string cppName=globalNames->getCpp(name);
	
	auto funcNames=globalNames->makeChild();
	
	string prototype;
	
	prototype+=getTypeCode(returnType);
	
	prototype+=" "+cppName+"(";
	
	bool keepTuplesTogether=true;
	
	if (keepTuplesTogether)
	{
		if (leftIn->isCreatable())
		{
			prototype+=getTypeCode(leftIn)+" me";
			funcNames->addPn("me");
		}
		
		if (rightIn->isCreatable())
		{
			if (leftIn->isCreatable())
				prototype+=", ";
			
			prototype+=getTypeCode(rightIn)+" in";
			funcNames->addPn("in");
		}
	}
	else
	{
		vector<std::pair<string, string>> args;
		
		if (leftIn->getType()==TypeBase::TUPLE)
		{
			for (auto i: *leftIn->getAllSubTypes())
			{
				args.push_back({getTypeCode(i.type), i.name});
			}
		}
		else if (!leftIn->isCreatable())
		{
			// do nothing
		}
		else
		{
			args.push_back({getTypeCode(leftIn), "me"});
		}
		
		if (rightIn->getType()==TypeBase::TUPLE)
		{
			for (auto i: *rightIn->getAllSubTypes())
			{
				args.push_back({getTypeCode(i.type), i.name});
			}
		}
		else if (!rightIn->isCreatable())
		{
			// do nothing
		}
		else
		{
			args.push_back({getTypeCode(rightIn), "in"});
		}
		
		for (int i=0; i<int(args.size()); i++)
		{
			if (i)
				prototype+=", ";
			
			prototype+=args[i].first;
			
			prototype+=" ";
			
			funcNames->addPn(args[i].second, args[i].second);
			
			prototype+=funcNames->getCpp(args[i].second);
		}
	}
	
	prototype+=")";
	
	activeFunc=CppFunc(new CppFuncBase(prototype, funcNames, returnType->isCreatable()));
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
	
	if (!globalTopCode.empty())
		out+=globalTopCode+"\n";
	
	if (!globalIncludesCode.empty())
		out+=globalIncludesCode+"\n";
	
	if (!globalTypesCode.empty())
		out+=globalTypesCode+"\n";
	
	if (funcs.size()>1)
	{
		for (auto i: funcs)
		{
			if (i.first!="main")
				out+=i.second->getPrototype()+";\n";
		}
		
		out+="\n";
	}
	
	if (!globalVarCode.empty())
		out+=globalVarCode+"\n";
	
	for (auto i: funcs)
	{
		out+=i.second->getPrototype();
		
		string funcSrc=i.second->getSource();
		
		if (funcSrc.size()<2)
		{
			out+="\n{\n\t// empty function\n}\n\n";
		}
		else
		{
			if (i.second->getIfReturnsVal() && funcSrc[0]!='{' && searchInString(funcSrc, ";")==int(funcSrc.size())-2)
				funcSrc="return "+funcSrc;
			out+="\n{\n";
			if (!i.second->varDeclareSource.empty())
				out+=indentString(i.second->varDeclareSource+"\n", indent);
			out+=indentString(funcSrc, indent);
			if (!out.empty() && out.back()!='\n')
				out+=";\n";
			//if (i.first=="main")
			//	out+=indentString("return 0;\n", indent);
			//if (out.substr(out.size()-2, 2)!=";\n")
			//	out+=";\n";
			out+="}\n\n";
		}
	}
	
	out+=
"int main(int argcIn, char** argvIn)\n\
{\n\
	argc = argcIn;\n\
	argv = argvIn;\n\
	if (argc >= 2 && strcmp(argv[1], \"--running-from-pinecone\") == 0)\n\
	{\n\
		argc -= 2;\n\
		if (argc == 0)\n\
			argv = 0;\n\
		else\n\
			argv += 2;\n\
	}\n\
	_main();\n\
	return 0;\n\
}\n";
	
	return out;
}


