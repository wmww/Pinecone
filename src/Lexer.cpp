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
	
	Token::Type getTokenType(char c, Token::Type previousType);
	
private:
	void setUp();
	
	CharClassifier::Type get(char c)
	{
		if (!hasSetUp)
			setUp();
		
		auto i=hm.find(c);
		
		if (i==hm.end())
			return UNKNOWN;
		else
			return i->second;
	}
	
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
		string str=(*i)->getText();
		for (unsigned j=0; j<str.size(); j++)
		{
			hm[str[j]]=OPERATOR;
		}
	}
	
	hasSetUp=true;
}

Token::Type CharClassifier::getTokenType(char c, Token::Type previousType)
{
	CharClassifier::Type type=get(c);
	
	if (previousType==Token::COMMENT)
	{
		if (type==NEWLINE)
			return Token::WHITESPACE;
		else
			return Token::COMMENT;
	}
	
	if (type==SINGLE_LINE_COMMENT)
	{
		return Token::COMMENT;
	}
	
	if (type==WHITESPACE)
		return Token::WHITESPACE;
	
	if (type==OPERATOR)
		return Token::OPERATOR;
	
	if (type==DIGIT || type==LETTER)
	{
		if (previousType==Token::IDENTIFIER || previousType==Token::LITERAL)
			return previousType;
		else if (type==DIGIT)
			return Token::LITERAL;
		else
			return Token::IDENTIFIER;
	}
	
	return Token::UNKNOWN;
}

void lexString(string text, vector<Token>& tokens)
{
	
}




