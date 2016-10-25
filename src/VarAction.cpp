#include "../h/VarAction.h"
#include "../h/ErrorHandler.h"
#include "../h/StackFrame.h"

VarGetAction::VarGetAction(size_t in, Type typeIn, string textIn):
	Action(typeIn, Void, Void, textIn)
{
	offset=in;
	
	setDescription("get " + typeIn->getName() + " '" + textIn + "'");
}

void* VarGetAction::execute(void* inLeft, void* inRight)
{
	void* out=malloc(returnType->getSize());
	memcpy(out, stackPtr+offset, returnType->getSize());
	return out;
}

VarSetAction::VarSetAction(size_t in, Type typeIn, string textIn):
	Action(typeIn, Void, typeIn, textIn)
{
	offset=in;
	
	setDescription("set " + typeIn->getName() + " '" + textIn + "'");
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

LiteralGetAction::LiteralGetAction(void* in, Type typeIn, string textIn):
	Action(typeIn, Void, Void, textIn)
{
	data=malloc(returnType->getSize());
	memcpy(data, in, returnType->getSize());
	
	setDescription(textIn);// + " (" + typeIn.toString() + " literal)");
}

LiteralGetAction::~LiteralGetAction()
{
	free(data);
}

void* LiteralGetAction::execute(void* inLeft, void* inRight)
{
	void* out=malloc(returnType->getSize());
	memcpy(out, data, returnType->getSize());
	return out;
}

