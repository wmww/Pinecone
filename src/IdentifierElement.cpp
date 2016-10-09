
#include "../h/IdentifierElement.h"
#include "../h/ErrorHandler.h"

#include <string>
using std::to_string;

IdentifierElement * IdentifierElement::makeNew(ElementData dataIn)
{
	return new IdentifierElement(dataIn);
}

string IdentifierElement::getReadableName()
{
	if (ptr)
		return ptr->getName() + " (" + ptr->getType().toString() + ")";
	else
		return data.text + " [unresolved]";
}

void IdentifierElement::resolveIdentifiers(IdentifierTable& table)
{
	ptr=table.getIdentifier(data.text);
}

Type IdentifierElement::getReturnType()
{
	if (ptr)
	{
		return ptr->getType();
	}
	else
	{
		return Type();
	}
}

