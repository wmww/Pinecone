#pragma once

#include "Type.h"

class CppProgram
{
public:
	string getTypeName(Type) {return "typeName";};
	
	void addCode(string in);
	void addComment(string in);
	void pushBlock();
	void popBlock();
	
	
	string getCode() {return code;}
	
private:
	string indent="    ";
	int indentationLevel=0;
	string code;
};
