#pragma once

#include "Identifier.h"
#include "Type.h"

#include <list>

using std::list;

class IdentifierTable
{
public:
	//should only be called within this class except for creating the root
	IdentifierTable(IdentifierTable * parentIn=nullptr) {parent=parentIn;}
	
	Identifier * getIdentifier(string nameIn);
	
	//you MUST be sure the same identifier doesn't already exist
	Identifier * makeIdentifier(string nameIn);
	
	//Identifier * getOrMakeIdentifier(string nameIn);
	
	void clear();
	
private:
	IdentifierTable * parent=nullptr;
	list<Identifier> identifiers;
};

