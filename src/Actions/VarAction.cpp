#include "../../h/Action.h"
#include "../../h/ErrorHandler.h"
#include "../../h/StackFrame.h"

class VarGetAction: public Action
{
public:
	
	VarGetAction(size_t in, Type typeIn, string textIn):
		Action(typeIn, Void, Void)
	{
		offset=in;
		
		setDescription("get " + typeIn->getName() + " '" + textIn + "'");
	}
	
	string getCSource(string inLeft, string inRight)
	{
		return "/* variable: "+getDescription()+" */";
	}
	
	void* execute(void* inLeft, void* inRight)
	{
		void* out=malloc(returnType->getSize());
		memcpy(out, stackPtr+offset, returnType->getSize());
		return out;
	}
	
private:
	
	size_t offset;
};

//an action for setting a variable, will NOT delete the data element in destructor
class VarSetAction: public Action
{
public:
	
	VarSetAction(size_t in, Type typeIn, string textIn):
		Action(typeIn, Void, typeIn)
	{
		offset=in;
		
		setDescription("set " + typeIn->getName() + " '" + textIn + "'");
	}
	
	string getCSource(string inLeft, string inRight)
	{
		return "/* var="+inRight+" */";
	}
	
	void* execute(void* left, void* right)
	{
		//copy data on to the stack location of the var
		memcpy(stackPtr+offset, right, inRightType->getSize());
		
		//return a new copy of the data
		void* out=malloc(returnType->getSize());
		memcpy(out, stackPtr+offset, inRightType->getSize());
		return out;
	}
	
private:
	
	size_t offset;
};

class ConstGetAction: public Action
{
public:
	
	ConstGetAction(void* in, Type typeIn, string textIn):
		Action(typeIn, Void, Void)
	{
		data=malloc(returnType->getSize());
		memcpy(data, in, returnType->getSize());
		
		setDescription(textIn);// + " (" + typeIn.toString() + " literal)");
	}

	~ConstGetAction()
	{
		free(data);
	}
	
	string getCSource(string inLeft, string inRight)
	{
		return "/* const get not yet implemented */";
	}
	
	void* execute(void* inLeft, void* inRight)
	{
		void* out=malloc(returnType->getSize());
		memcpy(out, data, returnType->getSize());
		return out;
	}
	
private:
	
	void* data;
};

ActionPtr varGetAction(size_t in, Type typeIn, string textIn)
{
	return ActionPtr(new VarGetAction(in, typeIn, textIn));
}

ActionPtr varSetAction(size_t in, Type typeIn, string textIn)
{
	return ActionPtr(new VarSetAction(in, typeIn, textIn));
}

ActionPtr constGetAction(void* in, Type typeIn, string textIn)
{
	return ActionPtr(new ConstGetAction(in, typeIn, textIn));
}
