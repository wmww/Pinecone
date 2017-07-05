#include "../../h/Action.h"
#include "../../h/ErrorHandler.h"

class LoopAction: public ActionData
{
public:
	
	LoopAction(Action conditionIn, Action endActionIn, Action loopActionIn)
		:ActionData(Void, Void, Void)
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
		string body=loopAction->getDescription();
		if (endAction!=voidAction)
		{
			vector<string> data={body, endAction->getDescription()};
			body=str::makeList(data);
		}
		return str::makeRootUpBinaryTree("@", condition->getReturnType()->getName(), "", condition->getDescription(), body);
		//return "while " + condition->getDescription() + " do " + loopAction->getDescription();
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
	
	/*
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
	*/
	
	void addToProg(Action inLeft, Action inRight, CppProgram* prog)
	{
		prog->code("while ");
		
		prog->pushExpr();
			condition->addToProg(prog);
		prog->popExpr();
		
		prog->pushBlock();
			loopAction->addToProg(prog);
			prog->endln();
			if (endAction!=voidAction)
			{
				endAction->addToProg(prog);
				prog->endln();
			}
		prog->popBlock();
	}
	
private:
	
	Action condition;
	Action loopAction;
	Action endAction;
};

Action loopAction(Action conditionIn, Action loopActionIn)
{
	return Action(new LoopAction(conditionIn, voidAction, loopActionIn));
}

Action loopAction(Action conditionIn, Action endActionIn, Action loopActionIn)
{
	return Action(new LoopAction(conditionIn, endActionIn, loopActionIn));
}


