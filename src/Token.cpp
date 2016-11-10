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

string stringFromTokens(vector<Token>& tokens)
{
	string out;
	
	for (unsigned i=0; i<tokens.size(); ++i)
	{
		out+=tokens[i]->getText() + "\t";
		if (tokens[i]->getType()==TokenData::OPERATOR)
		{
			if (tokens[i]->getOp())
			{
				out+=tokens[i]->getOp()->getText()+" ";
			}
			else
			{
				out+="unknown ";
			}
		}
		out+=TokenData::typeToString(tokens[i]->getType());
		if (i<tokens.size()-1)
			out+="\n";
	}
	
	return out;
}

