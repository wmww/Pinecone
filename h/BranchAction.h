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

class RightBranchAction: public Action
{
public:
	RightBranchAction(ActionPtr actionIn, ActionPtr rightInputIn);
	
	string getDescription();
	
	void* execute(void* inLeft, void* inRight);
	
private:
	ActionPtr action;
	ActionPtr rightInput;
};

class LeftBranchAction: public Action
{
public:
	LeftBranchAction(ActionPtr leftInputIn, ActionPtr actionIn);
	
	string getDescription();
	
	void* execute(void* inLeft, void* inRight);
	
private:
	ActionPtr leftInput;
	ActionPtr action;
};

