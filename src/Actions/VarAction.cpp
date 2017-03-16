#include "../../h/Action.h"
#include "../../h/ErrorHandler.h"
#include "../../h/StackFrame.h"

class VarGetAction: public ActionData
{
public:
	
	VarGetAction(size_t in, void ** stackPtrPtrIn, Type typeIn, string textIn):
		ActionData(typeIn, Void, Void)
	{
		offset=in;
		stackPtrPtr=stackPtrPtrIn;
		
		setDescription("get " + typeIn->getString() + " '" + textIn + "'");
	}
	
	string getCSource(string inLeft, string inRight)
	{
		return "/* variable: "+getDescription()+" */";
	}
	
	void* execute(void* inLeft, void* inRight)
	{
		if (!(*stackPtrPtr))
		{
			throw PineconeError("something fucked up big time. VarGetAction::execute called while stack pointer is still null", RUNTIME_ERROR);
		}
		
		void* out=malloc(returnType->getSize());
		memcpy(out, (char*)(*stackPtrPtr)+offset, returnType->getSize());
		return out;
	}
	
private:
	
	void ** stackPtrPtr;
	size_t offset;
};

class VarSetAction: public ActionData
{
public:
	
	VarSetAction(size_t in, void ** stackPtrPtrIn, Type typeIn, string textIn):
		ActionData(typeIn, Void, typeIn)
	{
		offset=in;
		stackPtrPtr=stackPtrPtrIn;
		
		setDescription("set " + typeIn->getString() + " '" + textIn + "'");
	}
	
	string getCSource(string inLeft, string inRight)
	{
		return "/* var="+inRight+" */";
	}
	
	void* execute(void* left, void* right)
	{
		if (!(*stackPtrPtr))
		{
			throw PineconeError("something fucked up big time. VarSetAction::execute called while stack pointer is still null", RUNTIME_ERROR);
		}
		
		//copy data on to the stack location of the var
		memcpy((char*)(*stackPtrPtr)+offset, right, inRightType->getSize());
		
		//return a new copy of the data
		void* out=malloc(returnType->getSize());
		memcpy(out, (char*)(*stackPtrPtr)+offset, inRightType->getSize());
		return out;
	}
	
private:
	
	void ** stackPtrPtr;
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
	
	void* execute(void* inLeft, void* inRight)
	{
		void* out=malloc(returnType->getSize());
		memcpy(out, data, returnType->getSize());
		return out;
	}
	
	void addCppCodeToProg(Action inLeft, Action inRight, CppProgram* prog)
	{
		getReturnType()->addInstToProg(data, prog);
	}
	
private:
	
	void* data;
};

Action varGetAction(size_t in, Type typeIn, string textIn)
{
	return Action(new VarGetAction(in, &stackPtr, typeIn, textIn));
}

Action varSetAction(size_t in, Type typeIn, string textIn)
{
	return Action(new VarSetAction(in, &stackPtr, typeIn, textIn));
}

Action globalGetAction(size_t in, Type typeIn, string textIn)
{
	return Action(new VarGetAction(in, &globalFramePtr, typeIn, textIn));
}

Action globalSetAction(size_t in, Type typeIn, string textIn)
{
	return Action(new VarSetAction(in, &globalFramePtr, typeIn, textIn));
}

Action constGetAction(void* in, Type typeIn, string textIn)
{
	return Action(new ConstGetAction(in, typeIn, textIn));
}
