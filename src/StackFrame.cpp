#include "../h/StackFrame.h"
#include "../h/msclStringFuncs.h"
#include "../h/ErrorHandler.h"

void * stackPtr=nullptr;

void StackFrame::addMember(Type in)
{
	members.push_back(in);
	frameSize+=in->getSize();
}

void StackFrame::addLeftInput(Type in)
{
	leftInputAdded=true;
	leftInputOffset=frameSize;
	addMember(in);
}

void StackFrame::addRightInput(Type in)
{
	rightInputAdded=true;
	rightInputOffset=frameSize;
	addMember(in);
}
