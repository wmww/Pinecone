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
	return returnType->cloneVoidPtr(stackPtr+offset);
}

void* VarSetAction::execute(void* left, void* right)
{
	returnType->setVoidPtr(stackPtr+offset, right);
	return returnType->cloneVoidPtr(stackPtr+offset);
}

