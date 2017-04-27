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
		return str::makeRootUpBinaryTree("?", condition->getReturnType()->getName(), "", condition->getDescription(), ifAction->getDescription());
		//return "if " + condition->getDescription() + " then " + ifAction->getDescription();
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
	}
	
private:
	
	Action condition;
	Action ifAction;
};

class IfElseAction: public ActionData
{
public:
	
	IfElseAction(Action conditionIn, Action ifActionIn, Action elseActionIn)
		:ActionData([&](){return ifActionIn->getReturnType()->matches(elseActionIn->getReturnType())?ifActionIn->getReturnType():Void;}(), Void, Void)
	{
		returnVal=getReturnType()!=Void;
		condition=conditionIn;
		ifAction=ifActionIn;
		elseAction=elseActionIn;
		
		if (condition->getReturnType()!=Bool)
		{
			error.log("IfElseAction created with condition action that does not return Bool", INTERNAL_ERROR);
		}
		
		if (condition->getInLeftType()!=Void || condition->getInRightType()!=Void)
		{
			error.log("IfElseAction created with conditiofn action that takes in something other then Void", INTERNAL_ERROR);
		}
		
		if (ifAction->getInLeftType()!=Void || ifAction->getInRightType()!=Void)
		{
			error.log("IfElseAction created with action that takes in something other then Void", INTERNAL_ERROR);
		}
	}

	string getDescription()
	{
		string branch=str::makeRootUpBinaryTree("╭┴╮\n│ │", "fls", "tru", elseAction->getDescription(), ifAction->getDescription());
		return str::makeRootUpBinaryTree("?", condition->getReturnType()->getName(), "", condition->getDescription(), branch);
		//return "if " + condition->getDescription() + " then " + ifAction->getDescription();
	}

	/*
	string getCSource(string inLeft, string inRight)
	{
		return "if (" + condition->getCSource() + ")\n{\n" + ifAction->getCSource() + "\n} else {\n" + elseAction->getCSource() + "\n}";
	}
	*/
	
	void* execute(void* inLeft, void* inRight)
	{
		void* out;
		void* conditionOut=condition->execute(nullptr, nullptr);
		if (*((bool*)conditionOut))
		{
			out=ifAction->execute(nullptr, nullptr);
		}
		else
		{
			out=elseAction->execute(nullptr, nullptr);
		}
		free(conditionOut);
		if (returnVal)
		{
			return out;
		}
		else
		{
			free(out);
			return nullptr;
		}
	}
	
	void addToProg(Action inLeft, Action inRight, CppProgram* prog)
	{
		if (returnVal && prog->getExprLevel()>0)
		{
			prog->pushExpr();
				condition->addToProg(voidAction, voidAction, prog);
			prog->popExpr();
			prog->code(" ? ");
			prog->pushExpr();
				ifAction->addToProg(voidAction, voidAction, prog);
			prog->popExpr();
			prog->code(" : ");
			prog->pushExpr();
				elseAction->addToProg(voidAction, voidAction, prog);
			prog->popExpr();
		}
		else
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
	}
	
private:
	
	Action condition;
	Action ifAction;
	Action elseAction;
	bool returnVal=false;
};

Action ifAction(Action conditionIn, Action ifActionIn)
{
	return Action(new IfAction(conditionIn, ifActionIn));
}

Action ifElseAction(Action conditionIn, Action ifActionIn, Action elseAction)
{
	return Action(new IfElseAction(conditionIn, ifActionIn, elseAction));
}


