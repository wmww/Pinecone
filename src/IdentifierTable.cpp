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

Identifier * IdentifierTable::getOrMakeIdentifier(string nameIn, Type type)
{
	Identifier * ptr=getIdentifier(nameIn);
	
	if (ptr)
	{
		if (ptr->getType()==type)
		{
			return ptr;
		}
		else
		{
			return nullptr;
		}
	}
	else
	{
		identifiers.push_back(Identifier(nameIn, /*identifiers.size(),*/ type, this));
		return &identifiers.back();
	}
}

void IdentifierTable::clear()
{
	identifiers.clear();
}

