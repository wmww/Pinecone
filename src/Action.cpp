#include "../h/Action.h"
#include "../h/StackFrame.h"
#include "../h/ErrorHandler.h"

ActionPtr voidAction;

Action::Action(Type returnTypeIn, Type inLeftTypeIn, Type inRightTypeIn, string textIn)
{
	text=textIn;
	returnType=returnTypeIn;
	inLeftType=inLeftTypeIn;
	inRightType=inRightTypeIn;
	
	if (!returnType || !inLeftType || !inRightType)
	{
		error.log("Action created with null type", INTERNAL_ERROR);
	}
}

string Action::toString()
{
	return text;
	
	//return returnType->getName() + " <- " + inLeftType->getName() + " " + text + " " + inRightType->getName();
}

void* VarGetAction::execute(void* inLeft, void* inRight)
{
	void* out=malloc(returnType->getSize());
	memcpy(out, stackPtr+offset, returnType->getSize());
	return out;
}

void* VarSetAction::execute(void* left, void* right)
{
	//copy data on to the stack location of the var
	memcpy(stackPtr+offset, right, inRightType->getSize());
	
	//return a new copy of the data
	void* out=malloc(returnType->getSize());
	memcpy(out, stackPtr+offset, inRightType->getSize());
	return out;
}

