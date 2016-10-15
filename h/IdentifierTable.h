#pragma once

//#include "Identifier.h"
#include "Type.h"
#include "Action.h"
#include "ElementData.h"

#include <list>

using std::list;

class ActionTable
{
public:
	//should only be called within this class except for creating the root
	ActionTable(ActionTable * parentIn=nullptr) {parent=parentIn;}
	
	~ActionTable() {clear();}
	
	Action * getBestAction(ElementData data, Type leftIn, Type rightIn);
	
	void addAction(Action * in) {actions.push_back(in);}
	
	//Identifier * getOrMakeIdentifier(string nameIn);
	
	void clear();
	
private:
	void addActionsToList(list<Action *>& in, string& text);
	Action * resolveOverload(list<Action *>& in, Type leftIn, Type rightIn);
	
	ActionTable * parent=nullptr;
	list<Action *> actions;
};

