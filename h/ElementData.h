#pragma once

#include <string>
using std::string;

struct ElementData
{
	enum Type
	{
		WHITESPACE,
		IDENTIFIER,
		LITERAL,
		OPERATOR,
		COMMENT,
		SCOPE,
		CAST,
		UNKNOWN
	};
	
	ElementData(string textIn, string fileIn, int lineIn, Type typeIn)
	{
		text=textIn;
		file=fileIn;
		line=lineIn;
		type=typeIn;
	}
	
	ElementData()
	{
		text="UNKNOWN_TEXT";
		file="UNKNOWN_FILE";
		line=-1;
		type=UNKNOWN;
	}
		
	string text;
	string file;
	int line;
	Type type;
	
	static string toString(Type);
	string toString();
};

