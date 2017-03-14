#include "../../h/Action.h"
#include "../../h/ErrorHandler.h"

class IfAction: public ActionData
{
public:
	
	IfAction(Action conditionIn, Action ifActionIn)
		:ActionData(Void, Void, Void)
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
	
	/*
	string getCSource(string inLeft, string inRight)
	{
		return "if (" + condition->getCSource() + ")\n{\n" + ifAction->getCSource() + "\n}";
	}
	*/

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
	
	Action condition;
	Action ifAction;
};

class IfElseAction: public ActionData
{
public:
	
	IfElseAction(Action conditionIn, Action ifActionIn, Action elseActionIn)
		:ActionData(Void, Void, Void)
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

	/*
	string getCSource(string inLeft, string inRight)
	{
		return "if (" + condition->getCSource() + ")\n{\n" + ifAction->getCSource() + "\n} else {\n" + elseAction->getCSource() + "\n}";
	}
	*/
	
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
	
	Action condition;
	Action ifAction;
	Action elseAction;
};

Action ifAction(Action conditionIn, Action ifActionIn)
{
	return Action(new IfAction(conditionIn, ifActionIn));
}

Action ifElseAction(Action conditionIn, Action ifActionIn, Action elseAction)
{
	return Action(new IfElseAction(conditionIn, ifActionIn, elseAction));
}


