#pragma once

#include "Type.h"

#include <map>

#include <unordered_set>

// Pinecone names that are hardcoded should start with '-' so that collisions can't happen with input code

/* naming conventions for hardcoded Pineocne name prefixes
these go in frount of any names that are hardcoded for the transpiler, since they are not valid identifier
cars they can not collide with names from pn source and they will be removed before they get to C++ source

global funcs: $
global vars: *
local vars: -
types: surounded by {}

*/

class CppNameContainer
{
public:
	static shared_ptr<CppNameContainer> makeRoot();
	shared_ptr<CppNameContainer> makeChild();
	void addPn(const string& pn, const string& cppNameHint="<- the value of that pn string please"); // will throw an error if pnName already exists
	void reserveCpp(const string& cpp);
	bool hasPnMe(const string& pn);
	string getCpp(const string& pn); // will throw an error if the Pinecone name doesn't exist
	CppNameContainer* getParent() {return parent;}
	
private:
	
	CppNameContainer();
	bool hasCpp(const string& cpp);
	bool hasCppMe(const string& cpp);
	bool hasCppUp(const string& cpp);
	bool hasCppDown(const string& cpp);
	
	std::unordered_set<string> cppSet;
	std::map<string, string> pnToCppMap;
	CppNameContainer* parent=nullptr;
	vector<shared_ptr<CppNameContainer>> children;
};

class CppFuncBase
{
public:
	CppFuncBase(string prototypeIn, shared_ptr<CppNameContainer> myNames);
	
	void code(const string& in);
	void name(const string& in); // converts a Pinecone name to a posibly different C++ name
	void line(const string& in);
	void endln();
	void comment(const string& in);
	void pushExpr();
	void popExpr();
	void pushBlock();
	void popBlock();
	string pnToCpp(const string& in);
	int getExprLevel() {return exprLevel;}
	bool getIfFreshLine() {return freshLine;}
	int getBlockLevel() {return blockLevel;}
	
	string getSource() {return source;}
	string getPrototype() {return prototype;}
	
private:
	string indent="    ";
	bool freshLine=true;
	int blockLevel=0;
	int exprLevel=0;
	
	string source;
	string prototype;
	
	vector<shared_ptr<CppNameContainer>> namespaceStack;
	
	friend CppProgram;
};

typedef shared_ptr<CppFuncBase> CppFunc;

class CppProgram
{
public:
	
	CppProgram();
	
	void code(const string& in)		{activeFunc->code(in);}
	void name(const string& in)		{activeFunc->name(in);}
	void line(const string& in)		{activeFunc->line(in);}
	void endln()					{activeFunc->endln();}
	void comment(const string& in)	{activeFunc->comment(in);}
	void pushExpr()					{activeFunc->pushExpr();}
	void popExpr()					{activeFunc->popExpr();}
	void pushBlock()				{activeFunc->pushBlock();}
	void popBlock()					{activeFunc->popBlock();}
	string pnToCpp(const string& in){return activeFunc->pnToCpp(in);}
	int getExprLevel()				{return activeFunc->getExprLevel();}
	
	
	void setup();
	string getTypeCode(Type in);
	void declareVar(const string& nameIn, Type typeIn, string initialValue="");
	void declareGlobal(const string& nameIn, Type typeIn, string initialValue="");
	bool hasFunc(const string& name);
	void addFunc(const string&, vector<std::pair<string, string>> args, string returnType, string contents);
	void pushFunc(const string&, vector<std::pair<string, string>> args, Type returnType);
	//void pushFunc(const string&, vector<NamedType> args, Type returnType);
	void popFunc();
	
	string getCppCode();
	
	shared_ptr<CppNameContainer> getGlobalNames() {return globalNames;};
	
private:
	
	string indent="    ";
	string globalCode;
	CppFunc activeFunc;
	vector<string> funcStack;
	std::map<string, CppFunc> funcs;
	shared_ptr<CppNameContainer> globalNames;
};

void addToProgPnStr(CppProgram * prog);
void addToProgCStr(CppProgram * prog);
void addToProgSubStr(CppProgram * prog);
void addToProgIntToStr(CppProgram * prog);
void addToProgDoubleToStr(CppProgram * prog);
void addToProgConcatStr(CppProgram * prog);
void addToProgAsciiToStr(CppProgram * prog);
string doubleToString(double in);

