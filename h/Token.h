#pragma once

#include "Operator.h"

#include <string>
using std::string;

#include <vector>
using std::vector;

//represents a single token such as an Int literal, an operator, or an identifier
//is immutable
class Token
{
public:
	
	enum Type
	{
		WHITESPACE,
		IDENTIFIER,
		LITERAL,
		OPERATOR,
		COMMENT,
		SCOPE,
		CAST,
		TUPLE,
		UNKNOWN
	};
	
	Token(string textIn, string fileIn, int lineIn, int charPosIn, Type tokenTypeIn)
	{
		text=textIn;
		file=fileIn;
		line=lineIn;
		charPos=charPosIn;
		tokenType=tokenTypeIn;
	}
	
	Token()
	{
		text="";
		file="";
		line=-1;
		charPos=-1;
		tokenType=UNKNOWN;
	}
	
	string getText() {return text;}
	string getFile() {return file;}
	int getLine() {return line;}
	int getCharPos() {return charPos;}
	Token::Type getType() {return tokenType;}
	Operator getOperator() {return op;}
	
	static string typeToString(Token::Type in);
	
private:
	
	string text;
	string file;
	int line;
	int charPos;
	Type tokenType;
	Operator op;
	
	static string toString(Type);
	string toString();
};

string stringFromTokens(vector<Token>& tokens);
