
#include "../../h/Action.h"
#include "../../h/ErrorHandler.h"
#include "../../h/StackFrame.h"
#include "../../h/AstNode.h"
#include "../../h/utils/stringNumConversion.h"
#include "../../h/utils/stringUtils.h"

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
	
	FunctionAction(AstNode nodeIn, Type returnTypeIn, shared_ptr<StackFrame> stackFameIn):
		ActionData(returnTypeIn, stackFameIn->getLeftInType(), stackFameIn->getRightInType())
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
			// the objects that are being returned are being destroyed before the function exits
			// also another problem: if a function returns a thing but nothing accepts the output, the destructor is not called
			//throw PineconeError("function body returns "+action->getReturnType()->getString()+" instead of "+returnType->getString(), SOURCE_ERROR, node->getToken());
			throw PineconeError("function body returns "+action->getReturnType()->getString()+" instead of "+returnType->getString()+"\n"+action->getDescription(), SOURCE_ERROR, node->getToken());
		}
		
		if (!action->getInLeftType()->isVoid() || !action->getInRightType()->isVoid())
		{
			throw PineconeError(action->getDescription() + " put into function even though its inputs are not void", INTERNAL_ERROR);
		}
	}
	
	string getDescription()
	{
		return str::putStringInTreeNodeBox("call func "+nameHint);
		//if (!action)
		//	resolveAction();
		//return "func: " + description;//action->getDescription();
	}
	
	bool isFunction() {return true;}
	
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
	
	void addToProg(Action inLeft, Action inRight, CppProgram* prog)
	{
		if (!action)
		{
			resolveAction();
		}
		
		// construct a unique name that will NEVER collide with another funtion (even in the case of overloaded functions with the same Pinecone name)
		// note that this name will be used to determine if this function has already been used in C++, so it should generate the same name every time
		string name="%";
		
		name+=(nameHint.empty() ? "func" : nameHint);
		
		name+="_"+str::ptrToUniqueStr(&*action);
		
		//name+="_"+getInLeftType()->getCompactString()+"_"+getInRightType()->getCompactString();
		
		/*
		if (str::hasSuffix(nameHint, "_CPP"))
			name=nameHint.substr(0, nameHint.size()-4);
		*/
		
		if (!prog->hasFunc(name))
		{
			prog->pushFunc(name, getInLeftType(), getInRightType(), getReturnType());
				if (getReturnType()->isCreatable() && action->getReturnType()!=getReturnType())
				{
					cppTupleCastAction(action, getReturnType())->addToProg(prog);
				}
				else
				{
					action->addToProg(prog);
				}
				prog->endln();
			prog->popFunc();
		}
		
		prog->name(name);
		
		prog->pushExpr();
			/*
			bool hasStarted=false;
			if (getInLeftType()->getType()==TypeBase::TUPLE)
			{
				for (auto i: *getInLeftType()->getAllSubTypes())
				{
					if (hasStarted)
						prog->code(", ");
					hasStarted=true;
					getElemFromTupleAction(getInLeftType(), i.name)->addToProg(inLeft, voidAction, prog);
				}
			}
			else if (!getInLeftType()->isCreatable())
			{
				// do nothing
			}
			else
			{
				if (hasStarted)
					prog->code(", ");
				hasStarted=true;
				inLeft->addToProg(prog);
			}
			
			if (getInRightType()->getType()==TypeBase::TUPLE)
			{
				for (auto i: *getInRightType()->getAllSubTypes())
				{
					if (hasStarted)
						prog->code(", ");
					hasStarted=true;
					getElemFromTupleAction(getInRightType(), i.name)->addToProg(inRight, voidAction, prog);
				}
			}
			else if (!getInRightType()->isCreatable())
			{
				// do nothing
			}
			else
			{
				if (hasStarted)
					prog->code(", ");
				hasStarted=true;
				inRight->addToProg(prog);
			}
			//prog->code(", ");
			//inRight->addToProg(prog);
			*/
			
			if (getInLeftType()->isCreatable())
			{
				inLeft->addToProg(prog);
			}
			
			if (getInRightType()->isCreatable())
			{
				if (getInLeftType()->isCreatable())
					prog->code(", ");
				
				inRight->addToProg(prog);
			}
			
		prog->popExpr();
		
		//prog->comment("function transpiling not yet implemented");
		
		
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

Action functionAction(AstNode nodeIn, Type returnTypeIn, shared_ptr<StackFrame> stackFameIn)
{
	return Action(new FunctionAction(move(nodeIn), returnTypeIn, stackFameIn));
}

