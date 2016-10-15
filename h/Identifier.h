#pragma once

/*
#include "Type.h"
#include "Action.h"

#include <vector>
using std::vector;

#include <string>
using std::string;

class IdentifierTable;

class Identifier
{
public:
	
	~Identifier();
	
	string getName() {return name;}
	vector<Action *>& getActions() {return actions;}
	Action * getAction(int index) {return actions[index];}
	void addAction(Action * in) {actions.push_back(in);}
	IdentifierTable * getTable() {return table;}
	
protected:
	
	friend IdentifierTable;
	
	//should only be called from within IdentifierTable
	Identifier(string nameIn, IdentifierTable * tableIn)
	{
		name=nameIn;
		table=tableIn;
	}
	
	string name;
	vector<Action *> actions;
	IdentifierTable * table;
};
*/
