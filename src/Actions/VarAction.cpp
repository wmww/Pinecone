#include "../../h/Action.h"
#include "../../h/ErrorHandler.h"
#include "../../h/StackFrame.h"
#include "../../h/CppProgram.h"

class VarGetAction: public ActionData
{
public:
	
	VarGetAction(size_t in, void ** stackPtrPtrIn, Type typeIn, string idIn):
		ActionData(typeIn, Void, Void)
	{
		offset=in;
		stackPtrPtr=stackPtrPtrIn;
		id=idIn;
		
		setDescription("get " + typeIn->getString() + " '" + idIn + "'");
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
	
	void addToProg(Action inLeft, Action inRight, CppProgram* prog)
	{
		prog->declareVar(id, getInRightType());
		prog->name(id);
	}
	
private:
	
	void ** stackPtrPtr;
	size_t offset;
	string id; // used for C++ transpiler
};

class VarSetAction: public ActionData
{
public:
	
	VarSetAction(size_t in, void ** stackPtrPtrIn, Type typeIn, string idIn):
		ActionData(typeIn, Void, typeIn)
	{
		offset=in;
		stackPtrPtr=stackPtrPtrIn;
		id=idIn;
		
		setDescription("set " + typeIn->getString() + " '" + idIn + "'");
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
	
	void addToProg(Action inLeft, Action inRight, CppProgram* prog)
	{
		prog->declareVar(id, getInRightType());
		prog->name(id);
		prog->code(" = ");
		prog->pushExpr();
		inRight->addToProg(prog);
		prog->popExpr();
		//if (prog->getExprLevel()==0)
		//	prog->endln();
	}
	
private:
	
	void ** stackPtrPtr;
	size_t offset;
	string id; // used for C++ transpiler
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
	
	void addToProg(Action inLeft, Action inRight, CppProgram* prog)
	{
		prog->code(getReturnType()->getCppLiteral(data, prog));
	}
	
private:
	
	void* data;
};

Action varGetAction(size_t in, Type typeIn, string textIn)
{
	return Action(new VarGetAction(in, &stackPtr, typeIn, textIn));
}

Action varSetAction(size_t in, Type typeIn, string varNameIn)
{
	return Action(new VarSetAction(in, &stackPtr, typeIn, varNameIn));
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
