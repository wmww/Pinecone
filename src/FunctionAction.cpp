
#include "../h/FunctionAction.h"
#include "../h/ErrorHandler.h"
#include "../h/StackFrame.h"

#include <cstring> //for memcpy
using std::memcpy;

class FunctionAction: public Action
{
public:
	
	FunctionAction(ActionPtr actionIn, Type inLeftTypeIn, Type inRightTypeIn, int dataSizeIn, string textIn):
		Action(actionIn->getReturnType(), inLeftTypeIn, inRightTypeIn, textIn)
	{
		dataSize=dataSizeIn;
		action=actionIn;
		
		if (action->getInLeftType()!=Void || action->getInRightType()!=Void)
		{
			error.log(action->getText() + " put into function even though its inputs are not void", INTERNAL_ERROR);
		}
	}

	string getDescription()
	{
		return getText() + action->getDescription();
	}

	void* execute(void* inLeft, void* inRight)
	{
		unsigned char * oldStackPtr=stackPtr;
		stackPtr=new unsigned char[dataSize];
		
		if (inLeft)
			memcpy(stackPtr, inLeft, getInLeftType()->getSize());
		
		if (inRight)
			memcpy(stackPtr+getInLeftType()->getSize(), inRight, getInRightType()->getSize());
		
		void* out=action->execute(nullptr, nullptr);
		
		delete[] stackPtr;
		stackPtr=oldStackPtr;
		
		return out;
	}
	
private:
	
	int dataSize;
	ActionPtr action;
};

ActionPtr functionAction(ActionPtr actionIn, Type inLeftTypeIn, Type inRightTypeIn, int dataSizeIn, string textIn)
{
	return ActionPtr(new FunctionAction(actionIn, inLeftTypeIn, inRightTypeIn, dataSizeIn, textIn));
}
