#include "../h/IdentifierTable.h"
#include "../h/Identifier.h"
#include "../h/ErrorHandler.h"

Identifier * IdentifierTable::getIdentifier(string nameIn)
{
	for (auto i=identifiers.begin(); i!=identifiers.end(); ++i)
	{
		if ((*i).getName()==nameIn)
		{
			return &(*i);
		}
	}
	
	if (parent)
	{
		Identifier * ptr=parent->getIdentifier(nameIn);
		
		if (ptr)
			return ptr;
	}
	
	return nullptr;
}

Identifier * IdentifierTable::makeIdentifier(string nameIn)
{
	identifiers.push_back(Identifier(nameIn, this));
	return &identifiers.back();
}

void IdentifierTable::clear()
{
	identifiers.clear();
}

