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
		case LINE_END: return "newline";
		case IDENTIFIER: return "identifier";
		case LITERAL: return "literal";
		case OPERATOR: return "operator";
		case LINE_COMMENT: return "single line comment";
		case BLOCK_COMMENT: return "block comment";
		case SCOPE: return "scope";
		case UNKNOWN: return "UNKNOWN";
		default: return "ERROR GETTING TOKEN TYPE";
	}
}

string TokenData::getDescription()
{
	return to_string(getLine()) + ":" + to_string(getCharPos()) + " (" + TokenData::typeToString(tokenType) + " '" + getText() + "')";
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

string tableStringFromTokens(const vector<Token>& tokens, string tableName)
{
	string out="Name\tType\n";
	
	for (unsigned i=0; i<tokens.size(); ++i)
	{
		//string a=tokens[i]->getText();
		
		//for (int i=1; i<12; i++)
		//	a=replaceSubstring(a, "\n", "newline");
		
		out+=replaceSubstring(tokens[i]->getText(), "\n", "\\n") + "\t";
		
		//out+=a+"\t";
		
		out+=tokens[i]->getTypeDescription();
		
		if (i<tokens.size()-1)
			out+="\n";
	}
	
	return putStringInTable(out, tableName);
}

string stringFromTokens(const vector<Token>& tokens, int left, int right)
{
	string out;
	
	for (int i=left; i<=right; ++i)
	{
		out+=tokens[i]->getText();
		if (i<right)
			out+=" ";
	}
	
	return out;
}
