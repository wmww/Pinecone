#pragma once

#include "Action.h"

class IfAction: public Action
{
public:
	
	IfAction(ActionPtr conditionIn, ActionPtr ifActionIn, string textIn);
	
	string getDescription();
	
	void* execute(void* inLeft, void* inRight);
	
private:
	
	ActionPtr condition;
	ActionPtr ifAction;
};


