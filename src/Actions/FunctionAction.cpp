
#include "../../h/Action.h"
#include "../../h/ErrorHandler.h"
#include "../../h/StackFrame.h"
#include "../../h/AstNode.h"

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
		node=nullptr;
		
		setDescription("function ("+getInLeftType()->getString()+"."+getInRightType()->getString()+" > "+getReturnType()->getString()+")");
		
		if (action->getInLeftType()!=Void || action->getInRightType()!=Void)
		{
			throw PineconeError(action->getDescription() + " put into function even though its inputs are not void", INTERNAL_ERROR);
		}
	}
	
	FunctionAction(AstNode nodeIn, Type returnTypeIn, Type leftTypeIn, Type rightTypeIn, shared_ptr<StackFrame> stackFameIn):
		ActionData(returnTypeIn, leftTypeIn, rightTypeIn)
	{
		stackFame=stackFameIn;
		node=move(nodeIn);
		action=nullptr;
		
		setDescription("function ("+getInLeftType()->getString()+"."+getInRightType()->getString()+" > "+getReturnType()->getString()+")");
	}
	
	void resolveAction()
	{
		if (!node || action)
		{
			throw PineconeError("FunctionAction::resolveAction called when this action is in the wrong state", INTERNAL_ERROR);
		}
		
		action=node->getAction();
		
		if (!returnType->isVoid() && !returnType->matches(action->getReturnType()))
		{
			throw PineconeError("function body returns "+action->getReturnType()->getString()+" instead of "+returnType->getString(), SOURCE_ERROR, node->getToken());
		}
		
		if (!action->getInLeftType()->isVoid() || !action->getInRightType()->isVoid())
		{
			throw PineconeError(action->getDescription() + " put into function even though its inputs are not void", INTERNAL_ERROR);
		}
	}
	
	string getDescription()
	{
		if (!action)
			resolveAction();
		return "func: " + description;//action->getDescription();
	}
	
	bool isFunction() {return true;}
	
	string getCSource(string inLeft, string inRight)
	{
		if (!action)
			resolveAction();
		
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
		if (!action)
			resolveAction();
		
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
	Action action=nullptr;
	AstNode node=nullptr;
};

Action functionAction(Action actionIn, shared_ptr<StackFrame> stackFameIn)
{
	return Action(new FunctionAction(actionIn, stackFameIn));
}

Action functionAction(AstNode nodeIn, Type returnTypeIn, Type leftTypeIn, Type rightTypeIn, shared_ptr<StackFrame> stackFameIn)
{
	return Action(new FunctionAction(move(nodeIn), returnTypeIn, leftTypeIn, rightTypeIn, stackFameIn));
}

