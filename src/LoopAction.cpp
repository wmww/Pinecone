#include "../h/LoopAction.h"
#include "../h/ErrorHandler.h"

LoopAction::LoopAction(ActionPtr conditionIn, ActionPtr loopActionIn)
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

string LoopAction::getDescription()
{
	return "while " + condition->getDescription() + " do " + loopAction->getDescription();
}

void* LoopAction::execute(void* inLeft, void* inRight)
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




