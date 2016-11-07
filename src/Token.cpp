#include "../h/Token.h"

string Token::typeToString(Token::Type in)
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
		out+=tokens[i].getText() + "   (" + Token::typeToString(tokens[i].getType()) + ")";
		if (i<tokens.size()-1)
			out+="\n";
	}
	
	return out;
}

