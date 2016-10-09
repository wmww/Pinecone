#include "../h/CastElement.h"


void CastElement::printToString(string& in, int depth)
{
	printIndentationToString(in, depth);
	in+="cast to ";
	in+=type.toString();
	in+="\n";
	
	if (input)
		input->printToString(in, depth+1);
}



