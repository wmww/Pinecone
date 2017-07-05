#pragma once

#include "Operator.h"

#include <string>
using std::string;

#include <vector>
using std::vector;

class SourceFile;

//represents a single token such as an Int literal, an operator, or an identifier
//is immutable
class TokenData
{
public:
	
	enum Type
	{
		WHITESPACE,
		LINE_END,
		IDENTIFIER,
		LITERAL,
		STRING_LITERAL,
		OPERATOR,
		LINE_COMMENT,
		BLOCK_COMMENT,
		SCOPE,
		CAST,
		TUPLE,
		UNKNOWN
	};
	
	TokenData(string textIn, shared_ptr<SourceFile> fileIn, int lineIn, int charPosIn, Type tokenTypeIn, Operator opIn=Operator(nullptr))
	{
		text=textIn;
		file=fileIn;
		line=lineIn;
		charPos=charPosIn;
		tokenType=tokenTypeIn;
		op=opIn;
	}
	
	string getText() const {return text;}
	shared_ptr<SourceFile> getFile() const {return file;}
	int getLine() const {return line;}
	int getCharPos() const {return charPos;}
	TokenData::Type getType() const {return tokenType;}
	Operator getOp() const {return op;}
	bool isComment() {return tokenType==LINE_COMMENT || tokenType==BLOCK_COMMENT;}
	
	static string typeToString(TokenData::Type in);
	string getDescription();
	string getTypeDescription();
	
private:
	
	string text;
	shared_ptr<SourceFile> file;
	int line;
	int charPos;
	Type tokenType;
	Operator op;
};

typedef shared_ptr<TokenData> Token;

Token makeToken(string textIn, shared_ptr<SourceFile> fileIn, int lineIn, int charPosIn, TokenData::Type tokenTypeIn, Operator opIn=Operator(nullptr));
Token makeToken(string textIn);

string stringFromTokens(const vector<Token>& tokens, int left, int right);
string tableStringFromTokens(const vector<Token>& tokens, string tableName);
