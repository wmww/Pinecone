#include "../h/Token.h"

Token makeToken(string textIn, string fileIn, int lineIn, int charPosIn, TokenData::Type tokenTypeIn, Operator opIn)
{
	return Token(new TokenData(textIn, fileIn, lineIn, charPosIn, tokenTypeIn, opIn));
}

string TokenData::typeToString(TokenData::Type in)
{
	switch (in)
	{
		case WHITESPACE: return "whitespace";
		case IDENTIFIER: return "identifier";
		case LITERAL: return "literal";
		case OPERATOR: return "operator";
		case COMMENT: return "comment";
		case SCOPE: return "scope";
		case UNKNOWN: return "UNKNOWN";
		default: return "ERROR GETTING TOKEN TYPE";
	}
}

string TokenData::getDescription()
{
	string out;
	
	if (tokenType==TokenData::OPERATOR)
	{
		if (op)
		{
			out+=op->getText()+" ";
		}
		else
		{
			out+="unknown ";
		}
	}
	
	out+=TokenData::typeToString(tokenType);
	
	return out;
}

string stringFromTokens(vector<Token>& tokens)
{
	string out;
	
	for (unsigned i=0; i<tokens.size(); ++i)
	{
		out+=tokens[i]->getText() + "\t";
		
		out+=tokens[i]->getDescription();
		
		if (i<tokens.size()-1)
			out+="\n";
	}
	
	return out;
}

