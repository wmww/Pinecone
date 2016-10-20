#include "../h/Action.h"
#include "../h/StackFrame.h"

void* VarGetAction::execute(void* inLeft, void* inRight)
{
	return returnType.cloneVoidPtr(stackPtr+offset);
}

void* VarSetAction::execute(void* left, void* right)
{
	returnType.setVoidPtr(stackPtr+offset, right);
	return returnType.cloneVoidPtr(stackPtr+offset);
}

