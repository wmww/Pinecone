#include "../h/ElementData.h"
#include "../h/ErrorHandler.h"

string ElementData::toString(Type in)
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
		default: return "ERROR GETTING ELEM TYPE";
	}
}

string ElementData::toString()
{
	return text+" ("+toString(type)+")";
}

