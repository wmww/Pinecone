#include "../../h/Action.h"

#include "../../h/ErrorHandler.h"

Action::Action(Type returnTypeIn, Type inLeftTypeIn, Type inRightTypeIn)
{
	returnType=returnTypeIn;
	inLeftType=inLeftTypeIn;
	inRightType=inRightTypeIn;
	
	if (!returnType || !inLeftType || !inRightType)
	{
		error.log("Action created with null type", INTERNAL_ERROR);
	}
}

string Action::toString()
{
	return description;
	
	//return returnType->getName() + " <- " + inLeftType->getName() + " " + text + " " + inRightType->getName();
}

//LambdaAction::LambdaAction(Type returnTypeIn, function<void*(void*,void*)> lambdaIn, Type inLeftTypeIn, Type inRightTypeIn, string textIn)
	
//void* LambdaAction::execute(void* inLeft, void* inRight)

class VoidAction: public Action
{
public:
	VoidAction(): Action(Void, Void, Void)
	{
		setDescription("Void Action");
	}
	
	string getCSource(string inLeft, string inRight)
	{
		return "/* void action */";
	}
	
	void* execute(void* inLeft, void* inRight)
	{
		return nullptr;
	}
};

class LambdaAction: public Action
{
public:
	LambdaAction(Type returnTypeIn, function<void*(void*,void*)> lambdaIn, Type inLeftTypeIn, Type inRightTypeIn, string textIn): Action(returnTypeIn, inLeftTypeIn, inRightTypeIn)
	{
		lambda=lambdaIn;
		setDescription(textIn);// + " (lambda action)");
	}
	
	string getCSource(string inLeft, string inRight)
	{
		return "/* lambda action (not yet implemented) */";
	}
	
	void* execute(void* inLeft, void* inRight)
	{
		return lambda(inLeft, inRight);
	}
	
private:
	function<void*(void*,void*)> lambda;
};

ActionPtr lambdaAction(Type returnTypeIn, function<void*(void*,void*)> lambdaIn, Type inLeftTypeIn, Type inRightTypeIn, string textIn)
{
	return ActionPtr(new LambdaAction(returnTypeIn, lambdaIn, inLeftTypeIn, inRightTypeIn, textIn));
}

ActionPtr createNewVoidAction()
{
	return ActionPtr(new VoidAction());
}
