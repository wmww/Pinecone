#pragma once

#include "Action.h"


//an action for getting a variable, will delete the data element in destructor
class VarGetAction: public Action
{
public:
	
	VarGetAction(size_t in, Type typeIn, string textIn);
	
	void* execute(void* inLeft, void* inRight);
	
private:
	
	size_t offset;
};

//an action for setting a variable, will NOT delete the data element in destructor
class VarSetAction: public Action
{
public:
	
	VarSetAction(size_t in, Type typeIn, string textIn);
	
	void* execute(void* left, void* right);
	
private:
	
	size_t offset;
};

class ConstGetAction: public Action
{
public:
	
	ConstGetAction(void* in, Type typeIn, string textIn);
	
	~ConstGetAction();
	
	void* execute(void* inLeft, void* inRight);
	
private:
	
	void* data;
};

