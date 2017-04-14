#include "../../h/Action.h"
#include "../../h/ErrorHandler.h"

/*
class AndAction: public ActionData
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
	
	void addToProg(Action inLeft, Action inRight, CppProgram* prog)
	{
		prog->code("if ");
		prog->pushExpr();
			condition->addToProg(voidAction, voidAction, prog);
		prog->popExpr();
		prog->pushBlock();
			ifAction->addToProg(voidAction, voidAction, prog);
			prog->endln();
		prog->popBlock();
		prog->code("else");
		prog->pushBlock();
			elseAction->addToProg(voidAction, voidAction, prog);
			prog->endln();
		prog->popBlock();
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

*/
