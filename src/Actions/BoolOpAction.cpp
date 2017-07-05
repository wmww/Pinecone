#include "../../h/Action.h"
#include "../../h/ErrorHandler.h"
#include "../../h/utils/stringDrawing.h"

class AndAction: public ActionData
{
public:
	
	AndAction(Action firstActionIn, Action secondActionIn)
		:ActionData(Bool, Void, Void)
	{
		firstAction=firstActionIn;
		secondAction=secondActionIn;
		
		if (firstAction->getReturnType()!=Bool)
		{
			throw PineconeError("AndAction created with first action that does not return Bool", INTERNAL_ERROR);
		}
		
		if (secondAction->getReturnType()!=Bool)
		{
			throw PineconeError("AndAction created with second action that does not return Bool", INTERNAL_ERROR);
		}
	}

	string getDescription()
	{
		return str::makeRootUpBinaryTree("&&", firstAction->getReturnType()->getName(), secondAction->getReturnType()->getName(), firstAction->getDescription(), secondAction->getDescription());
		//return firstAction->getDescription() + " && " + firstAction->getDescription();
	}
	
	void* execute(void* inLeft, void* inRight)
	{
		bool* out=(bool*)malloc(sizeof(bool));
		*out=false;
		void* firstVal=firstAction->execute(nullptr, nullptr);
		
		if (*((bool*)firstVal))
		{
			void* secondVal=secondAction->execute(nullptr, nullptr);
			
			if (*((bool*)secondVal))
			{
				*out=true;
			}
			
			free(secondVal);
		}
		
		free(firstVal);
		
		return out;
	}
	
	void addToProg(Action inLeft, Action inRight, CppProgram* prog)
	{
		prog->pushExpr();
			firstAction->addToProg(voidAction, voidAction, prog);
		prog->popExpr();
		
		prog->code(" && ");
		
		prog->pushExpr();
			secondAction->addToProg(voidAction, voidAction, prog);
		prog->popExpr();
	}
	
private:
	
	Action firstAction;
	Action secondAction;
};

class OrAction: public ActionData
{
public:
	
	OrAction(Action firstActionIn, Action secondActionIn)
		:ActionData(Bool, Void, Void)
	{
		firstAction=firstActionIn;
		secondAction=secondActionIn;
		
		if (firstAction->getReturnType()!=Bool)
		{
			throw PineconeError("OrAction created with first action that does not return Bool", INTERNAL_ERROR);
		}
		
		if (secondAction->getReturnType()!=Bool)
		{
			throw PineconeError("OrAction created with second action that does not return Bool", INTERNAL_ERROR);
		}
	}

	string getDescription()
	{
		return str::makeRootUpBinaryTree("||", firstAction->getReturnType()->getName(), secondAction->getReturnType()->getName(), firstAction->getDescription(), secondAction->getDescription());
	}
	
	void* execute(void* inLeft, void* inRight)
	{
		bool* out=(bool*)malloc(sizeof(bool));
		*out=true;
		void* firstVal=firstAction->execute(nullptr, nullptr);
		
		if (!*((bool*)firstVal))
		{
			void* secondVal=secondAction->execute(nullptr, nullptr);
			
			if (!*((bool*)secondVal))
			{
				*out=false;
			}
			
			free(secondVal);
		}
		
		free(firstVal);
		
		return out;
	}
	
	void addToProg(Action inLeft, Action inRight, CppProgram* prog)
	{
		prog->pushExpr();
			firstAction->addToProg(voidAction, voidAction, prog);
		prog->popExpr();
		
		prog->code(" || ");
		
		prog->pushExpr();
			secondAction->addToProg(voidAction, voidAction, prog);
		prog->popExpr();
	}
	
private:
	
	Action firstAction;
	Action secondAction;
};

Action andAction(Action firstActionIn, Action secondActionIn)
{
	return Action(new AndAction(firstActionIn, secondActionIn));
}

Action orAction(Action firstActionIn, Action secondActionIn)
{
	return Action(new OrAction(firstActionIn, secondActionIn));
}


