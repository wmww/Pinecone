#pragma once

#include "Action.h"
#include <vector>

using std::vector;

class MakeTupleAction: public Action
{
public:
	
	MakeTupleAction(vector<ActionPtr>& sourceActionsIn);
	
	string getDescription();
	
	void* execute(void* inLeft, void* inRight);
	
private:
	
	vector<ActionPtr> sourceActions;
};

