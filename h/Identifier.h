#pragma once

#include "Type.h"
#include <string>
using std::string;

class IdentifierTable;

class Identifier
{
public:
	
	string getName() {return name;}
	//int getId() {return id;}
	Type getType() {return type;}
	IdentifierTable * getTable() {return table;}
	
	
	
protected:
	
	friend IdentifierTable;
	
	//should only be called from within IdentifierTable
	Identifier(string nameIn, /*int idIn,*/ Type typeIn, IdentifierTable * tableIn)
	{
		name=nameIn;
		//id=idIn;
		type=typeIn;
		table=tableIn;
	}
	
	string name;
	//int id;
	Type type={Type::UNKNOWN};
	IdentifierTable * table;
};

