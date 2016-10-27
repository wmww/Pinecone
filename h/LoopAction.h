#pragma once

#include "Action.h"

class LoopAction: public Action
{
public:
	
	LoopAction(ActionPtr conditionIn, ActionPtr loopActionIn);
	
	string getDescription();
	
	void* execute(void* inLeft, void* inRight);
	
private:
	
	ActionPtr condition;
	ActionPtr loopAction;
};



