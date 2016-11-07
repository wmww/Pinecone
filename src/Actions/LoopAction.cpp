#include "../../h/Action.h"
#include "../../h/ErrorHandler.h"

class LoopAction: public Action
{
public:
	
	LoopAction(ActionPtr conditionIn, ActionPtr loopActionIn)
		:Action(Void, Void, Void, "LOOP")
	{
		condition=conditionIn;
		loopAction=loopActionIn;
		
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

	void* execute(void* inLeft, void* inRight)
	{
		while (true)
		{
			void* conditionOut=condition->execute(nullptr, nullptr);
			if (!(*((bool*)conditionOut)))
				break;
			free(loopAction->execute(nullptr, nullptr));
			free(conditionOut);
		}
		
		return nullptr;
	}
	
private:
	
	ActionPtr condition;
	ActionPtr loopAction;
};

ActionPtr loopAction(ActionPtr conditionIn, ActionPtr loopActionIn)
{
	return ActionPtr(new LoopAction(conditionIn, loopActionIn));
}


