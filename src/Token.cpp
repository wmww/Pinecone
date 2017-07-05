#include "../h/Token.h"
#include "../h/msclStringFuncs.h"
#include "../h/utils/stringUtils.h"

Token makeToken(string textIn, shared_ptr<SourceFile> fileIn, int lineIn, int charPosIn, TokenData::Type tokenTypeIn, Operator opIn)
{
	if (str::hasPrefix(textIn, "\"") && !str::hasSuffix(textIn, "\""))
		textIn+="\"";
	return Token(new TokenData(textIn, fileIn, lineIn, charPosIn, tokenTypeIn, opIn));
}

Token makeToken(string textIn)
{
	return Token(new TokenData(textIn, nullptr, 0, 0, TokenData::IDENTIFIER, Operator(nullptr)));
}

string TokenData::typeToString(TokenData::Type in)
{
	switch (in)
	{
		case WHITESPACE: return "whitespace";
		case LINE_END: return "newline";
		case IDENTIFIER: return "identifier";
		case LITERAL: return "literal";
		case STRING_LITERAL: return "string literal";
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
	/*string out="Name\tType\n";
	
	for (unsigned i=0; i<tokens.size(); ++i)
	{
		out+=replaceSubstring(tokens[i]->getText(), "\n", "\\n") + "\t";
		
		out+=tokens[i]->getTypeDescription();
		
		if (i<tokens.size()-1)
			out+="\n";
	}
	
	return putStringInTable(out, tableName);*/
	
	vector<string> lines;
	string abv="", blw="";
	string str="";
	//const int tabSize=1;
	const int maxWidth=240;
	const string seporator="    ";
	//const string seporator="   |   ";
	
	for (unsigned i=0; i<tokens.size(); ++i)
	{
		//string next=" | ";
		
		//for (int j=0; j<int(tabSize-str.size()%tabSize); j++)
		//	next+=" ";
		
		//next+=tokens[i]->getText();
		
		if (i>0 && str.size()+seporator.size()+tokens[i]->getText().size()<maxWidth)
		{
			abv+=seporator;
			str+=seporator;
			blw+=seporator;
			
			for (unsigned j=0; j<tokens[i]->getText().size(); j++)
			{
				abv+=" ";
				blw+=" ";
			}
			
			str+=tokens[i]->getText();
		}
		else
		{
			if (i>0)
			{
				lines.push_back(abv);
				lines.push_back(str);
				lines.push_back(blw);
				lines.push_back("");
				abv="";
				blw="";
			}
			
			for (unsigned j=0; j<tokens[i]->getText().size(); j++)
			{
				abv+=" ";
				blw+=" ";
			}
			
			str=tokens[i]->getText();
		}
	}
	
	lines.push_back(abv);
	lines.push_back(str);
	lines.push_back(blw);
	
	return lineListToBoxedString(lines, tableName, -1, false, maxWidth+4);
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
