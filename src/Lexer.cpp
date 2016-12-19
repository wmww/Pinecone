#include "../h/Token.h"
#include "../h/ErrorHandler.h"
#include "../h/Operator.h"
#include "../h/AllOperators.h"

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
		MULTI_LINE_COMMENT_START,
		MULTI_LINE_COMMENT_END,
		UNKNOWN,
	};
	
	static TokenData::Type getTokenType(CharClassifier::Type type, TokenData::Type previousType);
	
	CharClassifier::Type get(string& str, int i);
	
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
	
	hm['\n']=NEWLINE;
	hm[';']=NEWLINE;
	
	for (char c='a'; c<='z'; ++c)
		hm[c]=LETTER;
	
	for (char c='A'; c<='Z'; ++c)
		hm[c]=LETTER;
	
	hm['_']=LETTER;
	
	for (char c='0'; c<='9'; ++c)
		hm[c]=DIGIT;
	
	hm['#']=SINGLE_LINE_COMMENT;
	
	unordered_map<string, Operator>& opsMap=ops->getOpsMap();
	
	for (auto i=opsMap.begin(); i!=opsMap.end(); ++i)
	{
		string str=(*i).first;
		for (unsigned j=0; j<str.size(); j++)
		{
			hm[str[j]]=OPERATOR;
		}
	}
	
	hasSetUp=true;
}

CharClassifier::Type CharClassifier::get(string& str, int index)
{
	//	set up the first time this function is called
	if (!hasSetUp)
		setUp();
	
	//	chack fo multi line comments in a special way, because they are multi character
	
	if (str.substr(index, 2)=="//")
		return MULTI_LINE_COMMENT_START;
	
	if (index>0 && str.substr(index-1, 2)=="\\\\")
		return MULTI_LINE_COMMENT_END;
	
	//	allow a . to be a digit character only if it is followed by a digit
	if (index<int(str.size())-1 && str[index]=='.')
	{
		auto i=hm.find(str[index+1]);
		
		if (i!=hm.end() && i->second==DIGIT)
			return DIGIT;
	}
	
	//	handle all other cases using the hashmap
	char c=str[index];
	
	auto i=hm.find(c);
	
	if (i==hm.end())
		return UNKNOWN;
	else
		return i->second;
}

TokenData::Type CharClassifier::getTokenType(CharClassifier::Type type, TokenData::Type previousType)
{
	if (previousType==TokenData::LINE_COMMENT)
	{
		if (type==NEWLINE)
			return TokenData::WHITESPACE;
		else
			return TokenData::LINE_COMMENT;
	}
	else if (previousType==TokenData::BLOCK_COMMENT)
	{
		if (type==MULTI_LINE_COMMENT_END)
			return TokenData::WHITESPACE;
		else
			return TokenData::BLOCK_COMMENT;
	}
	
	switch (type)
	{
		
	case SINGLE_LINE_COMMENT:
		return TokenData::LINE_COMMENT;
	
	case MULTI_LINE_COMMENT_START:
		return TokenData::BLOCK_COMMENT;
	
	case MULTI_LINE_COMMENT_END:
		error.log("block comment end without start", SOURCE_ERROR);
		return TokenData::UNKNOWN;
		
	case WHITESPACE:
		return TokenData::WHITESPACE;
	case NEWLINE:
		return TokenData::LINE_END;
		
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
		CharClassifier::Type charType=charClassifier.get(text, i);
		TokenData::Type newType=CharClassifier::getTokenType(charType, type);
		
		if (newType!=type)
		{
			if (!tokenTxt.empty())
			{
				if (type==TokenData::OPERATOR)
				{
					vector<Operator> opMatches;
					ops->get(tokenTxt, opMatches);
					
					for (auto op: opMatches)
					{
						tokens.push_back(makeToken(op->getText(), filename, line, charPos, type, op));
					}
				}
				else if (type==TokenData::LINE_COMMENT || type==TokenData::BLOCK_COMMENT)
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




