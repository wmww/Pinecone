#pragma once

#include "Type.h"

class CppProgram
{
public:
	string getTypeName(Type) {return "typeName";};
	
	void addCode(string in) {code+=in;};
	void addComment(string in) {code+="\n/*\n"+in+"\n/*\n";};
	
	string getCode() {return code;}
	
private:
	string code;
};
