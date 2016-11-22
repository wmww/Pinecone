#include "../../h/Action.h"
#include "../../h/ErrorHandler.h"

class LoopAction: public Action
{
public:
	
	LoopAction(ActionPtr conditionIn, ActionPtr endActionIn, ActionPtr loopActionIn)
		:Action(Void, Void, Void)
	{
		condition=conditionIn;
		loopAction=loopActionIn;
		endAction=endActionIn;
		
		if (condition->getReturnType()!=Bool)
		{
			error.log("LoopAction created with condition action that does not return Bool", INTERNAL_ERROR);
		}
		
		if (condition->getInLeftType()!=Void || condition->getInRightType()!=Void)
		{
			error.log("LoopAction created with condition action that takes in something other then Void", INTERNAL_ERROR);
		}
		
		if (loopAction->getInLeftType()!=Void || loopAction->getInRightType()!=Void)
		{
			error.log("LoopAction created with action that takes in something other then Void", INTERNAL_ERROR);
		}
	}

	string getDescription()
	{
		return "while " + condition->getDescription() + " do " + loopAction->getDescription();
	}
	
	string getCSource(string inLeft, string inRight)
	{
		string out;
		
		out+="while (";
		out+=condition->getCSource();
		out+=")\n{";
		out+=loopAction->getCSource();
		out+=endAction->getCSource();
		out+="\n}";
		
		return out;
	}
	
	void* execute(void* inLeft, void* inRight)
	{
		void* conditionOut;
		
		while (true)
		{
			conditionOut=condition->execute(nullptr, nullptr);
			if (!(*((bool*)conditionOut)))
				break;
			free(conditionOut);
			free(loopAction->execute(nullptr, nullptr));
			free(endAction->execute(nullptr, nullptr));
		}
		
		free(conditionOut);
		
		return nullptr;
	}
	
private:
	
	ActionPtr condition;
	ActionPtr loopAction;
	ActionPtr endAction;
};

ActionPtr loopAction(ActionPtr conditionIn, ActionPtr loopActionIn)
{
	return ActionPtr(new LoopAction(conditionIn, voidAction, loopActionIn));
}

ActionPtr loopAction(ActionPtr conditionIn, ActionPtr endActionIn, ActionPtr loopActionIn)
{
	return ActionPtr(new LoopAction(conditionIn, endActionIn, loopActionIn));
}


