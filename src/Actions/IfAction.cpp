#include "../../h/Action.h"
#include "../../h/ErrorHandler.h"

class IfAction: public Action
{
public:
	
	IfAction(ActionPtr conditionIn, ActionPtr ifActionIn)
		:Action(Void, Void, Void, "IF")
	{
		condition=conditionIn;
		ifAction=ifActionIn;
		
		if (condition->getReturnType()!=Bool)
		{
			error.log("IfAction created with condition action that does not return Bool", INTERNAL_ERROR);
		}
		
		if (condition->getInLeftType()!=Void || condition->getInRightType()!=Void)
		{
			error.log("IfAction created with condition action that takes in something other then Void", INTERNAL_ERROR);
		}
		
		if (ifAction->getInLeftType()!=Void || ifAction->getInRightType()!=Void)
		{
			error.log("IfAction created with action that takes in something other then Void", INTERNAL_ERROR);
		}
	}

	string getDescription()
	{
		return "if " + condition->getDescription() + " then " + ifAction->getDescription();
	}
	
	string getCSource(string inLeft, string inRight)
	{
		return "if (" + condition->getCSource() + ")\n{\n" + ifAction->getCSource() + "\n}";
	}

	void* execute(void* inLeft, void* inRight)
	{
		void* conditionOut=condition->execute(nullptr, nullptr);
		if (*((bool*)conditionOut))
		{
			free(ifAction->execute(nullptr, nullptr));
		}
		free(conditionOut);
		return nullptr;
	}
	
private:
	
	ActionPtr condition;
	ActionPtr ifAction;
};

class IfElseAction: public Action
{
public:
	
	IfElseAction(ActionPtr conditionIn, ActionPtr ifActionIn, ActionPtr elseActionIn)
		:Action(Void, Void, Void, "IF_ELSE")
	{
		condition=conditionIn;
		ifAction=ifActionIn;
		elseAction=elseActionIn;
		
		if (condition->getReturnType()!=Bool)
		{
			error.log("IfElseAction created with condition action that does not return Bool", INTERNAL_ERROR);
		}
		
		if (condition->getInLeftType()!=Void || condition->getInRightType()!=Void)
		{
			error.log("IfElseAction created with condition action that takes in something other then Void", INTERNAL_ERROR);
		}
		
		if (ifAction->getInLeftType()!=Void || ifAction->getInRightType()!=Void)
		{
			error.log("IfElseAction created with action that takes in something other then Void", INTERNAL_ERROR);
		}
	}

	string getDescription()
	{
		return "if " + condition->getDescription() + " then " + ifAction->getDescription();
	}

	string getCSource(string inLeft, string inRight)
	{
		return "if (" + condition->getCSource() + ")\n{\n" + ifAction->getCSource() + "\n} else {\n" + elseAction->getCSource() + "\n}";
	}
	
	void* execute(void* inLeft, void* inRight)
	{
		void* conditionOut=condition->execute(nullptr, nullptr);
		if (*((bool*)conditionOut))
		{
			free(ifAction->execute(nullptr, nullptr));
		}
		else
		{
			free(elseAction->execute(nullptr, nullptr));
		}
		free(conditionOut);
		return nullptr;
	}
	
private:
	
	ActionPtr condition;
	ActionPtr ifAction;
	ActionPtr elseAction;
};

ActionPtr ifAction(ActionPtr conditionIn, ActionPtr ifActionIn)
{
	return ActionPtr(new IfAction(conditionIn, ifActionIn));
}

ActionPtr ifElseAction(ActionPtr conditionIn, ActionPtr ifActionIn, ActionPtr elseAction)
{
	return ActionPtr(new IfElseAction(conditionIn, ifActionIn, elseAction));
}


