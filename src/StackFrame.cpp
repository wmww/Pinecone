#include "../h/StackFrame.h"
#include "../h/msclStringFuncs.h"
#include "../h/ErrorHandler.h"

void* globalFramePtr=nullptr;
void* stackPtr=nullptr;

void StackFrame::addMember(Type in)
{
	if (!in->isCreatable())
	{
		throw PineconeError("tried to insert uncreatable type "+in->getString()+" into stack frame", INTERNAL_ERROR);
	}
	
	members.push_back(in);
	frameSize+=in->getSize();
}

void StackFrame::setInput(Type left, Type right)
{
	if (inputSet)
	{
		error.log("StackFrame::setInput called multiple times", INTERNAL_ERROR);
	}
	else
	{
		if (!left->isVoid())
		{
			leftInputOffset=frameSize;
			leftInputIndex=members.size();
			addMember(left);
		}
		
		if (!right->isVoid())
		{
			rightInputOffset=frameSize;
			rightInputIndex=members.size();
			addMember(right);
		}
		
		inputSet=true;
	}
}

Type StackFrame::getLeftInType()
{
	if (leftInputIndex>=0)
	{
		return members[leftInputIndex];
	}
	else
	{
		return Void;
	}
}

Type StackFrame::getRightInType()
{
	if (rightInputIndex>=0)
	{
		return members[rightInputIndex];
	}
	else
	{
		return Void;
	}
}

size_t StackFrame::getLeftOffset()
{
	if (leftInputIndex>=0)
	{
		return leftInputOffset;
	}
	else
	{
		error.log("tried to get the left input offset from a stack frame that does not have a left input", INTERNAL_ERROR);
		return 0;
	}
}

size_t StackFrame::getRightOffset()
{
	if (rightInputIndex>=0)
	{
		return rightInputOffset;
	}
	else
	{
		throw PineconeError("tried to get the right input offset from a stack frame that does not have a right input", INTERNAL_ERROR);
	}
}

