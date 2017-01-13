
#include "../../h/Action.h"
#include "../../h/ErrorHandler.h"
#include "../../h/StackFrame.h"

#include <cstring> //for memcpy
using std::memcpy;

class FunctionAction: public ActionData
{
public:
	
	FunctionAction(Action actionIn, shared_ptr<StackFrame> stackFameIn):
		ActionData(actionIn->getReturnType(), stackFameIn->getLeftInType(), stackFameIn->getRightInType())
	{
		stackFame=stackFameIn;
		action=actionIn;
		
		setDescription("function ("+getInLeftType()->getString()+"."+getInRightType()->getString()+" > "+getReturnType()->getString()+")");
		
		if (action->getInLeftType()!=Void || action->getInRightType()!=Void)
		{
			throw PineconeError(action->getDescription() + " put into function even though its inputs are not void", INTERNAL_ERROR);
		}
	}

	string getDescription()
	{
		return "func: " + action->getDescription();
	}
	
	bool isFunction() {return true;}
	
	string getCSource(string inLeft, string inRight)
	{
		string out;
		out+="/* function C source not yet implemented, but here is the action: */";
		out+="\n*/\n";
		out+=action->getCSource();
		out+="\n*/";
		return out;
		
		/*string name=getGlobalCName();
		
		string globalCFunc;
		globalCFunc+=action->getReturnType()->getCSource();
		globalCFunc+=" ";
		globalCFunc+=name;
		globalCFunc+="("
		if (!inLeft.empty())
			globalCFunc+=inLeft+","
		globalCFunc+=
		globalCFunc+=
		globalCFunc+=
		action->getCSource()
		addGlobalCSource();*/
	}
	
	void* execute(void* inLeft, void* inRight)
	{
		void * oldStackPtr=stackPtr;
		
		stackPtr=malloc(stackFame->getSize());
		
		if (inLeft)
			memcpy((char*)stackPtr+stackFame->getLeftOffset(), inLeft, getInLeftType()->getSize());
		
		if (inRight)
			memcpy((char*)stackPtr+stackFame->getRightOffset(), inRight, getInRightType()->getSize());
		
		void* out=action->execute(nullptr, nullptr);
		
		free(stackPtr);
		stackPtr=oldStackPtr;
		
		return out;
	}
	
private:
	
	shared_ptr<StackFrame> stackFame;
	Action action;
};

Action functionAction(Action actionIn, shared_ptr<StackFrame> stackFameIn)
{
	return Action(new FunctionAction(actionIn, stackFameIn));
}
