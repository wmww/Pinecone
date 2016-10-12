#include "../h/Element.h"
#include "../h/msclStringFuncs.h"
#include "../h/ErrorHandler.h"

void Element::printToString(string& in, int depth)
{
	printIndentationToString(in, depth);
	in+=getReadableName();
	in+="\n";
}

void Element::printIndentationToString(string& in, int depth)
{
	for (int i=0; i<depth; ++i)
	{
		in+="   ";
	}
}

string Element::printToBoxedString(string name)
{
	string data;
	printToString(data);
	return putStringInBox(data, name);
}




