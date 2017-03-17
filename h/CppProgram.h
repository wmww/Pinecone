#pragma once

#include "Type.h"

#include <map>

#include <unordered_set>

class CppNameContainer
{
public:
	static shared_ptr<CppNameContainer> makeRoot();
	shared_ptr<CppNameContainer> makeChild();
	void addPn(const string& pn); // will throw an error if pnName already exists
	string getCppForPn(const string& pn); // will throw an error if the Pinecone name doesn't exist
	
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
	CppFuncBase(string prototypeIn, shared_ptr<CppNameContainer> globalNames);
	
	void code(const string& in);
	void line(const string& in);
	void endln();
	void comment(const string& in);
	void pushExpr();
	void popExpr();
	void pushBlock();
	void popBlock();
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
};

typedef shared_ptr<CppFuncBase> CppFunc;

class CppProgram
{
public:
	
	CppProgram();
	
	void code(const string& in)		{activeFunc->code(in);}
	void line(const string& in)		{activeFunc->line(in);}
	void endln()					{activeFunc->endln();}
	void comment(const string& in)	{activeFunc->comment(in);}
	void pushExpr()					{activeFunc->pushExpr();}
	void popExpr()					{activeFunc->popExpr();}
	void pushBlock()				{activeFunc->pushBlock();}
	void popBlock()					{activeFunc->popBlock();}
	int getExprLevel()				{return activeFunc->getExprLevel();}
	
	bool hasFunc(const string& name);
	void pushFunc(const string&, vector<NamedType> args, Type returnType);
	void popFunc();
	
	string getCppCode();
	
private:
	
	string indent="    ";
	CppFunc activeFunc;
	vector<string> funcStack;
	std::map<string, CppFunc> funcs;
	shared_ptr<CppNameContainer> globalNames;
};
