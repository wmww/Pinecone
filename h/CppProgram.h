#pragma once

#include "Type.h"
#include <map>

class CppFuncBase
{
public:
	CppFuncBase(string prototypeIn);
	
	void code(string in);
	void line(string in);
	void endln();
	void comment(string in);
	void pushExpr();
	void popExpr();
	void pushBlock();
	void popBlock();
	int getExprLevel() {return exprLevel;}
	
	string getSource() {return source;}
	
private:
	string indent="    ";
	bool freshLine=true;
	int indentLevel=0;
	int exprLevel=0;
	
	string source;
	string prototype;
};

typedef shared_ptr<CppFuncBase> CppFunc;

class CppProgram
{
public:
	
	CppProgram();
	
	string getTypeName(Type) {return "typeName";};
	
	void code(string in)		{activeFunc->code(in);}
	void line(string in)		{activeFunc->line(in);}
	void endln()				{activeFunc->endln();}
	void comment(string in)		{activeFunc->comment(in);}
	void pushExpr()				{activeFunc->pushExpr();}
	void popExpr()				{activeFunc->popExpr();}
	void pushBlock()			{activeFunc->pushBlock();}
	void popBlock()				{activeFunc->popBlock();}
	int getExprLevel()			{return activeFunc->getExprLevel();}
	
	void getIfFuncExists(string name);
	void pushFunc(string name, vector<NamedType> args, Type returnType);
	void popFunc();
	
	string getCppCode();
	
private:
	
	CppFunc activeFunc;
	vector<string> funcStack;
	std::map<string, CppFunc> funcs;
};
