#pragma once

#include "Operator.h"

#include <string>
using std::string;

#include <vector>
using std::vector;

//represents a single token such as an Int literal, an operator, or an identifier
//is immutable
class TokenData
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
	
	TokenData(string textIn, string fileIn, int lineIn, int charPosIn, Type tokenTypeIn, Operator opIn=Operator(nullptr))
	{
		text=textIn;
		file=fileIn;
		line=lineIn;
		charPos=charPosIn;
		tokenType=tokenTypeIn;
		op=opIn;
	}
	
	string getText() const {return text;}
	string getFile() const {return file;}
	int getLine() const {return line;}
	int getCharPos() const {return charPos;}
	TokenData::Type getType() const {return tokenType;}
	Operator getOp() const {return op;}
	
	static string typeToString(TokenData::Type in);
	string getDescription();
	string getTypeDescription();
	
private:
	
	string text;
	string file;
	int line;
	int charPos;
	Type tokenType;
	Operator op;
};

typedef shared_ptr<TokenData> Token;

Token makeToken(string textIn, string fileIn, int lineIn, int charPosIn, TokenData::Type tokenTypeIn, Operator opIn=Operator(nullptr));

string tableStringFromTokens(vector<Token>& tokens, string tableName);
