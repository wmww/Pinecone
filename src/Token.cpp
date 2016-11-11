#include "../h/Token.h"
#include "../h/msclStringFuncs.h"

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
	return text+" ("+TokenData::typeToString(tokenType)+")";
}

string TokenData::getTypeDescription()
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

string tableStringFromTokens(vector<Token>& tokens, string tableName)
{
	string out="Name\tType\n";
	
	for (unsigned i=0; i<tokens.size(); ++i)
	{
		out+=tokens[i]->getText() + "\t";
		
		out+=tokens[i]->getTypeDescription();
		
		if (i<tokens.size()-1)
			out+="\n";
	}
	
	return putStringInTable(out, tableName);
}

