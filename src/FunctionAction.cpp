
#include "../h/FunctionAction.h"
#include "../h/ErrorHandler.h"

#include <cstring> //for memcpy
using std::memcpy;

unsigned char * stackPtr=nullptr;

FunctionAction::FunctionAction(ActionPtr actionIn, Type inLeftTypeIn, Type inRightTypeIn, int dataSizeIn, string textIn):
	Action(actionIn->getReturnType(), inLeftTypeIn, inRightTypeIn, textIn)
{
	dataSize=dataSizeIn;
	action=actionIn;
	
	if (action->getInLeftType().getType()!=Type::VOID || action->getInRightType().getType()!=Type::VOID)
	{
		error.log(action->getText() + " put into function even though its inputs are not void", INTERNAL_ERROR);
	}
}

string FunctionAction::getDescription()
{
	return getText() + action->getDescription();
}

void* FunctionAction::execute(void* inLeft, void* inRight)
{
	unsigned char * oldStackPtr=stackPtr;
	stackPtr=new unsigned char[dataSize];
	
	if (inLeft)
		memcpy(stackPtr, inLeft, getInLeftType().getSize());
	
	if (inRight)
		memcpy(stackPtr+getInLeftType().getSize(), inRight, getInRightType().getSize());
	
	void* out=action->execute(nullptr, nullptr);
	
	delete[] stackPtr;
	stackPtr=oldStackPtr;
	
	return out;
}

