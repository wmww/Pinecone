#pragma once

#include "Action.h"

class BranchAction: public Action
{
public:
	BranchAction(ActionPtr leftInputIn, ActionPtr actionIn, ActionPtr rightInputIn);
	
	string getDescription();
	
	void* execute(void* inLeft, void* inRight);
	
private:
	ActionPtr action;
	ActionPtr leftInput;
	ActionPtr rightInput;
};
