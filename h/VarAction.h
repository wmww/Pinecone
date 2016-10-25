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

class LiteralGetAction: public Action
{
public:
	
	LiteralGetAction(void* in, Type typeIn, string textIn);
	
	~LiteralGetAction();
	
	void* execute(void* inLeft, void* inRight);
	
private:
	
	void* data;
};


