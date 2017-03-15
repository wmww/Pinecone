#pragma once

#include "Type.h"

class CppProgram
{
public:
	string getTypeName(Type) {return "typeName";};
	
	void addCode(string in);
	void addComment(string in);
	void pushExpression();
	void popExpression();
	void pushBlock();
	void popBlock();
	int getExpressionLevel() {return expressionLevel;}
	
	string getCode() {return code;}
	
private:
	string indent="    ";
	int indentationLevel=0;
	int expressionLevel=0;
	string code;
};
