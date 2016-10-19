#include "../h/CastElement.h"
#include "../h/ErrorHandler.h"

/*

void CastElement::printToString(string& in, int depth)
{
	printIndentationToString(in, depth);
	in+="cast to ";
	in+=type.toString();
	in+="\n";
	
	if (input)
		input->printToString(in, depth+1);
}

void* CastElement::execute()
{
	if (!input)
	{
		return nullptr;
	}
	
	void* ptr=input->execute();
	
	void* out=input->getReturnType().castVoidPtr(ptr, type);
	
	input->getReturnType().deleteVoidPtr(ptr);
	
	return out;
}

*/

