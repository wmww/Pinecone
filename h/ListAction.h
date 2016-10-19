#pragma once

#include "Action.h"
#include <list>

using std::list;

class ListAction: public Action
{
public:
	ListAction(list<ActionPtr>& actionsIn);
	
	string getDescription();
	
	void* execute(void* inLeft, void* inRight);
	
private:
	list<ActionPtr> actions;
};
