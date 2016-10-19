#pragma once

#include <string>
using std::string;

enum OperatorType
{
	OP_DOT,
	OP_PLUS,
	OP_MINUS,
	OP_COLON,
	
	//used for making arrays of all ovveridable operators
	OP_TYPE_OVERRIDEABLE_LAST,
	
	OP_OPEN,
	OP_CLOSE,
};

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
		text="";
		file="";
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

