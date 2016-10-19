
#pragma once

#include "Action.h"

class FunctionAction: public Action
{
public:
	
	FunctionAction(ActionPtr actionIn, Type inLeftTypeIn, Type inRightTypeIn, int dataSizeIn, string textIn);
	
	string getDescription();
	
	void* execute(void* inLeft, void* inRight);
	
private:
	
	int dataSize;
	ActionPtr action;
};

