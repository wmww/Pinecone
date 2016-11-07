#include "../h/Action.h"

#include "../h/ErrorHandler.h"

Action::Action(Type returnTypeIn, Type inLeftTypeIn, Type inRightTypeIn, string textIn)
{
	text=textIn;
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
	return text;
	
	//return returnType->getName() + " <- " + inLeftType->getName() + " " + text + " " + inRightType->getName();
}

//LambdaAction::LambdaAction(Type returnTypeIn, function<void*(void*,void*)> lambdaIn, Type inLeftTypeIn, Type inRightTypeIn, string textIn)
	
//void* LambdaAction::execute(void* inLeft, void* inRight)

class VoidAction: public Action
{
public:
	VoidAction(): Action(Void, Void, Void, "Void")
	{
		setDescription("Void Action");
	}
	
	void* execute(void* inLeft, void* inRight)
	{
		return nullptr;
	}
};

class LambdaAction: public Action
{
public:
	LambdaAction(Type returnTypeIn, function<void*(void*,void*)> lambdaIn, Type inLeftTypeIn, Type inRightTypeIn, string textIn): Action(returnTypeIn, inLeftTypeIn, inRightTypeIn, textIn)
	{
		lambda=lambdaIn;
		setDescription(text);// + " (lambda action)");
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
