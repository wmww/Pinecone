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
	
	static Token::Type getTokenType(CharClassifier::Type type, Token::Type previousType);
	
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
	
	for (auto i=OperatorBase::operators.begin(); i!=OperatorBase::operators.end(); ++i)
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

Token::Type CharClassifier::getTokenType(CharClassifier::Type type, Token::Type previousType)
{
	if (previousType==Token::COMMENT)
	{
		if (type==NEWLINE)
			return Token::WHITESPACE;
		else
			return Token::COMMENT;
	}
	
	switch (type)
	{
		
	case SINGLE_LINE_COMMENT:
		return Token::COMMENT;
		
	case WHITESPACE:
	case NEWLINE:
		return Token::WHITESPACE;
		
	case OPERATOR:
		return Token::OPERATOR;
		
	case LETTER:
	case DIGIT:
		if (previousType==Token::IDENTIFIER || previousType==Token::LITERAL)
			return previousType;
		else if (type==DIGIT)
			return Token::LITERAL;
		else
			return Token::IDENTIFIER;
		
	default:
		return Token::UNKNOWN;
		
	}
}

void lexString(string text, string filename, vector<Token>& tokens)
{
	string tokenTxt;
	int line=1;
	int charPos=1;
	
	Token::Type type=Token::WHITESPACE;
	
	for (unsigned i=0; i<text.size(); i++)
	{
		CharClassifier::Type charType=charClassifier.get(text[i]);
		Token::Type newType=CharClassifier::getTokenType(charType, type);
		
		if (newType!=type)
		{
			if (!tokenTxt.empty())
			{
				if (type==Token::OPERATOR)
				{
					vector<Operator> ops;
					getOperators(tokenTxt, ops);
					
					for (auto op: ops)
					{
						tokens.push_back(Token(tokenTxt, filename, line, charPos, type, op));
					}
				}
				else if (type==Token::COMMENT)
				{
					// do nothing
				}
				else
				{
					tokens.push_back(Token(tokenTxt, filename, line, charPos, type));
				}
			}
			tokenTxt="";
		}
		
		if (newType!=Token::WHITESPACE)
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




