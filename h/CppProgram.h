#pragma once

#include "Type.h"

class CppProgram
{
public:
	
	CppProgram();
	
	string getTypeName(Type) {return "typeName";};
	
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
};
