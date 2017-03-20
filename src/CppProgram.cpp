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

void CppNameContainer::reserveCpp(const string& cpp)
{
	if (hasCpp(cpp))
	{
		throw PineconeError("called CppNameContainer::reserveCpp with id '"+cpp+"', which already exists", INTERNAL_ERROR);
	}
	
	cppSet.insert(cpp);
}

bool CppNameContainer::hasPnMe(const string& pn)
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

CppFuncBase::CppFuncBase(string prototypeIn, shared_ptr<CppNameContainer> myNames)
{
	prototype=prototypeIn;
	namespaceStack.push_back(myNames);
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
	globalCode+="// this C++ code is transpiled from Pinecone\n";
	globalCode+="// Pinecone v"+to_string(VERSION_X)+"."+to_string(VERSION_Y)+"."+to_string(VERSION_Z)+" was used\n";
	globalCode+="\n";
	globalCode+="#include <string.h>\n";
	globalCode+="#include <stdlib.h>\n";
	globalCode+="#include <stdio.h>\n";
	globalCode+="\n";
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
	
	pushFunc(string("main"), {}, Int);
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
		return getTypeCode(in->getSubType())+" *";
		
	case TypeBase::TUPLE:
	{
		string compact="{"+in->getCompactString()+"}";
		
		if (!globalNames->hasPnMe(compact))
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
	
	if (activeFunc->namespaceStack.back()->hasPnMe(nameIn))
		return;
	
	activeFunc->namespaceStack.back()->addPn(nameIn, nameIn);
	
	code(getTypeCode(typeIn));
	code(" ");
	name(nameIn);
	if (!initialValue.empty())
	{
		code(" = ");
		code(initialValue);
	}
	endln();
}

void CppProgram::declareGlobal(const string& nameIn, Type typeIn, string initialValue)
{
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
	
	globalCode+=code;
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
	
	auto func=CppFunc(new CppFuncBase(prototype, globalNames->makeChild()));
	funcs[name]=func;
	func->code(contents);
}

void CppProgram::pushFunc(const string& name, vector<std::pair<string, string>> args, Type returnType)
{
	if (hasFunc(name))
	{
		throw PineconeError("called CppProgram::pushFunc with function name '"+name+"', which already exists", INTERNAL_ERROR);
	}
	
	globalNames->addPn(name, name);
	
	string cppName=globalNames->getCpp(name);
	
	auto funcNames=globalNames->makeChild();
	
	string prototype;
	
	prototype+=getTypeCode(returnType);
	
	prototype+=" "+cppName+"(";
	
	for (int i=0; i<int(args.size()); i++)
	{
		if (i)
			prototype+=", ";
		
		prototype+=args[i].first;
		
		prototype+=" ";
		
		funcNames->addPn(args[i].second, args[i].second);
		
		prototype+=funcNames->getCpp(args[i].second);
	}
	
	prototype+=")";
	
	activeFunc=CppFunc(new CppFuncBase(prototype, funcNames));
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


/// add common funcs

void addToProgPnStr(CppProgram * prog)
{
	if (!prog->hasFunc("$pnStr"))
	{
		prog->pushFunc("$pnStr", {{"const char *", "-in"}}, String);
			prog->declareVar("-i", Int, "0");
			prog->code("while ");
			prog->pushExpr();
				prog->name("-in");
				prog->code("[");
				prog->pushExpr();
					prog->name("-i");
				prog->popExpr();
				prog->code("]");
			prog->popExpr();
			prog->pushBlock();
				prog->name("-i");
				prog->code("++");
				prog->endln();
			prog->popBlock();
			prog->code("return ");
			prog->code(prog->getTypeCode(String));
			prog->pushExpr();
				prog->name("-i");
				prog->code(", ");
				prog->code("(unsigned char *)");
				prog->name("-in");
			prog->popExpr();
			prog->endln();
		prog->popFunc();
	}
}

void addToProgCStr(CppProgram * prog)
{
	if (!prog->hasFunc("$cStr"))
	{
		prog->pushFunc("$cStr", {{prog->getTypeCode(String), "-in"}}, Byte->getPtr());
			
			//prog->code("int strSize = ");
			//getElemFromTupleAction(String, "_size")->addToProg(voidAction, varGetAction(0, String, "in"), prog);
			//prog->endln();
			
			prog->declareVar("-tmp", Byte->getPtr());
			
			prog->name("-tmp");
			prog->code(" = (unsigned char*)malloc");
			prog->pushExpr();
				getElemFromTupleAction(String, "_size")->addToProg(voidAction, varGetAction(0, String, "-in"), prog);
				prog->code(" + 1");
			prog->popExpr();
			prog->endln();
			
			prog->code("memcpy");
			prog->pushExpr();
				prog->name("-tmp");
				prog->code(", ");
				prog->pushExpr();
					getElemFromTupleAction(String, "_data")->addToProg(voidAction, varGetAction(0, String, "-in"), prog);
				prog->popExpr();
				prog->code(", ");
				getElemFromTupleAction(String, "_size")->addToProg(voidAction, varGetAction(0, String, "-in"), prog);
			prog->popExpr();
			prog->endln();
			prog->name("-tmp");
			prog->code("[");
			prog->pushExpr();
				getElemFromTupleAction(String, "_size")->addToProg(voidAction, varGetAction(0, String, "-in"), prog);
			prog->popExpr();
			prog->code("] = 0");
			prog->endln();
			
			prog->code("return ");
			prog->name("-tmp");
			prog->endln();
		prog->popFunc();
	}
}

void addToProgIntToStr(CppProgram * prog)
{
	if (!prog->hasFunc("$intToStr"))
	{
		string strType=prog->getTypeCode(String);
		
		prog->addFunc("$intToStr", {{"int", "in"}}, prog->getTypeCode(String),
			"bool neg = in < 0;\n"
			"if (neg) in *= -1;\n"
			"int val = in;\n"
			"int size=0;\n"
			"while (val)\n"
			"{\n"
			"	size++;\n"
			"	val /= 10;\n"
			"}\n"
			"if (size == 0)\n\tsize = 1;\n"
			"if (neg)\n\tsize++;\n"
			"unsigned char * data = (unsigned char *)malloc(size);\n"
			"val = in;\n"
			"for (int i=0; i<(neg ? size-1 : size); i++)\n"
			"{\n"
			"	data[size-i-1] = (val % 10) + '0';\n"
			"	val /= 10;\n"
			"}\n"
			"if (neg)\n\tdata[0] = '-';\n"
			"return "+strType+"(size, data);\n"
		);
	}
}

void addToProgConcatStr(CppProgram * prog)
{
	if (!prog->hasFunc("$concatStr"))
	{
		string strType=prog->getTypeCode(String);
		
		prog->addFunc("$concatStr", {{strType, "a"}, {strType, "b"}}, strType,
			"int newSize = a._size + b._size;\n"
			"unsigned char * newData = (unsigned char *)malloc(newSize);\n"
			"memcpy(newData, a._data, a._size);\n"
			"memcpy(newData + a._size, b._data, b._size);\n"
			"return "+strType+"(newSize, newData);\n"
		);
	}
}

/*
void addToProgDoubleToString(CppProgram * prog)
{
	if (!prog->hasFunc("$doubleToStr"))
	{
		prog->pushFunc("$doubleToStr", {{"double", "-in"}}, String)
			prog->declareVar("-a", Int);
			
			prog->name("-a");
			prog->code(" = ");
			prog->name("-in");
			prog->endln();
			
			@c("if ");
			@p->pushExpr();
				@n("-in");
				@c(" < 0");
			@p->popExpr();
			@p->pushBlock();
				@n("-in");
				@c(" *= -1");
				@el();
			@->popBlock();
			
			@c("while ");
			@p->pushExpr();
				@n("-in");
				@c(" - (long long)");
				@n("-in");
				@c(" > 0.000000001");
			@p->popExpr();
			@p->pushBlock();
				@n("-in");
				@c(" *= 10");
				@el();
			@->popBlock();
			
			@c("return ");
		prog->popFunc();
	}
}
*/

void addToProgDoubleToString(CppProgram * prog)
{
	if (!prog->hasFunc("$doubleToStr"))
	{
		addToProgIntToStr(prog);
		addToProgConcatStr(prog);
		addToProgPnStr(prog);
		
		string intToStr=prog->pnToCpp("$intToStr");
		string concat=prog->pnToCpp("$concatStr");
		string pnStr=prog->pnToCpp("$pnStr");
		
		prog->addFunc("$doubleToStr", {{"double", "in"}}, prog->getTypeCode(String),
			"long long a=in;\n"
			"in-=a;\n"
			"if (in<0)\n"
			"	in*=-1;\n"
			"while (in-(long long)in>0.000000001)\n"
			"{\n"
			"	in*=10;\n"
			"}\n"
			"return "+concat+"("+concat+"("+intToStr+"(a), "+pnStr+"(\".\")), "+intToStr+"((int)in));\n"
		);
	}
}

string doubleToString(double in)
{
	long long a=in;
	in-=a;
	if (in<0)
		in*=-1;
	while (in-(long long)in>0.000000001)
	{
		in*=10;
	}
	return to_string(a)+"."+to_string((long long)in);
}

