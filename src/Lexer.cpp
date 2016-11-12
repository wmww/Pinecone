#include "../h/Token.h"
#include "../h/ErrorHandler.h"
#include "../h/Operator.h"

#include <vector>
using std::vector;

#include <unordered_map>
using std::unordered_map;

class CharClassifier
{
public:
	enum Type
	{
		WHITESPACE,
		NEWLINE,
		LETTER,
		DIGIT,
		OPERATOR,
		SINGLE_LINE_COMMENT,
		UNKNOWN
	};
	
	static TokenData::Type getTokenType(CharClassifier::Type type, TokenData::Type previousType);
	
	CharClassifier::Type get(char c);
	
private:
	void setUp();
	
private:
	unordered_map<char, CharClassifier::Type> hm;
	bool hasSetUp=false;
};

CharClassifier charClassifier;

void CharClassifier::setUp()
{
	hm[' ']=WHITESPACE;
	hm['\t']=WHITESPACE;
	hm[';']=WHITESPACE;
	
	hm['\n']=NEWLINE;
	
	for (char c='a'; c<='z'; ++c)
		hm[c]=LETTER;
	
	for (char c='A'; c<='Z'; ++c)
		hm[c]=LETTER;
	
	hm['_']=LETTER;
	
	for (char c='0'; c<='9'; ++c)
		hm[c]=DIGIT;
	
	hm['#']=SINGLE_LINE_COMMENT;
	
	for (auto i=OperatorData::operators.begin(); i!=OperatorData::operators.end(); ++i)
	{
		string str=(*i).first;
		for (unsigned j=0; j<str.size(); j++)
		{
			hm[str[j]]=OPERATOR;
		}
	}
	
	hasSetUp=true;
}

CharClassifier::Type CharClassifier::get(char c)
{
	if (!hasSetUp)
		setUp();
	
	auto i=hm.find(c);
	
	if (i==hm.end())
		return UNKNOWN;
	else
		return i->second;
}

TokenData::Type CharClassifier::getTokenType(CharClassifier::Type type, TokenData::Type previousType)
{
	if (previousType==TokenData::COMMENT)
	{
		if (type==NEWLINE)
			return TokenData::WHITESPACE;
		else
			return TokenData::COMMENT;
	}
	
	switch (type)
	{
		
	case SINGLE_LINE_COMMENT:
		return TokenData::COMMENT;
		
	case WHITESPACE:
	case NEWLINE:
		return TokenData::WHITESPACE;
		
	case OPERATOR:
		return TokenData::OPERATOR;
		
	case LETTER:
	case DIGIT:
		if (previousType==TokenData::IDENTIFIER || previousType==TokenData::LITERAL)
			return previousType;
		else if (type==DIGIT)
			return TokenData::LITERAL;
		else
			return TokenData::IDENTIFIER;
		
	default:
		return TokenData::UNKNOWN;
		
	}
}

void lexString(string text, string filename, vector<Token>& tokens)
{
	string tokenTxt;
	int line=1;
	int charPos=1;
	
	TokenData::Type type=TokenData::WHITESPACE;
	
	for (unsigned i=0; i<text.size(); i++)
	{
		CharClassifier::Type charType=charClassifier.get(text[i]);
		TokenData::Type newType=CharClassifier::getTokenType(charType, type);
		
		if (newType!=type)
		{
			if (!tokenTxt.empty())
			{
				if (type==TokenData::OPERATOR)
				{
					vector<Operator> ops;
					getOperators(tokenTxt, ops);
					
					for (auto op: ops)
					{
						tokens.push_back(makeToken(op->getText(), filename, line, charPos, type, op));
					}
				}
				else if (type==TokenData::COMMENT)
				{
					// do nothing
				}
				else
				{
					tokens.push_back(makeToken(tokenTxt, filename, line, charPos, type));
				}
			}
			tokenTxt="";
		}
		
		if (newType!=TokenData::WHITESPACE)
		{
			tokenTxt+=text[i];
		}
		
		type=newType;
		
		if (charType==CharClassifier::NEWLINE)
		{
			line++;
			charPos=1;
		}
		else
		{
			charPos++;
		}
	}
}




