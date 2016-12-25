#include "../../h/Action.h"
#include "../../h/ErrorHandler.h"
#include "../../h/StackFrame.h"

class VarGetAction: public ActionData
{
public:
	
	VarGetAction(size_t in, Type typeIn, string textIn):
		ActionData(typeIn, Void, Void)
	{
		offset=in;
		
		setDescription("get " + typeIn->getString() + " '" + textIn + "'");
	}
	
	string getCSource(string inLeft, string inRight)
	{
		return "/* variable: "+getDescription()+" */";
	}
	
	void* execute(void* inLeft, void* inRight)
	{
		void* out=malloc(returnType->getSize());
		memcpy(out, (char*)stackPtr+offset, returnType->getSize());
		return out;
	}
	
private:
	
	size_t offset;
};

//an action for setting a variable, will NOT delete the data element in destructor
class VarSetAction: public ActionData
{
public:
	
	VarSetAction(size_t in, Type typeIn, string textIn):
		ActionData(typeIn, Void, typeIn)
	{
		offset=in;
		
		setDescription("set " + typeIn->getString() + " '" + textIn + "'");
	}
	
	string getCSource(string inLeft, string inRight)
	{
		return "/* var="+inRight+" */";
	}
	
	void* execute(void* left, void* right)
	{
		if (!stackPtr)
		{
			throw PineconeError("something fucked up big time. VarSetAction::execute called while stack pointer is still null", RUNTIME_ERROR);
		}
		
		//copy data on to the stack location of the var
		memcpy((char*)stackPtr+offset, right, inRightType->getSize());
		
		//return a new copy of the data
		void* out=malloc(returnType->getSize());
		memcpy(out, (char*)stackPtr+offset, inRightType->getSize());
		return out;
	}
	
private:
	
	size_t offset;
};

class ConstGetAction: public ActionData
{
public:
	
	ConstGetAction(void* in, Type typeIn, string textIn):
		ActionData(typeIn, Void, Void)
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

Action varGetAction(size_t in, Type typeIn, string textIn)
{
	return Action(new VarGetAction(in, typeIn, textIn));
}

Action varSetAction(size_t in, Type typeIn, string textIn)
{
	return Action(new VarSetAction(in, typeIn, textIn));
}

Action constGetAction(void* in, Type typeIn, string textIn)
{
	return Action(new ConstGetAction(in, typeIn, textIn));
}
