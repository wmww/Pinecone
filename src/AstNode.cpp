#include "../h/AstNode.h"
#include "../h/ErrorHandler.h"

AstNode astVoid=AstNode(new AstVoid);

Type AstList::getReturnType(Namespace ns)
{
	if (nodes.empty())
	{
		return Void;
	}
	else
	{
		return nodes.back()->getReturnType(ns);
	}
}

Action AstList::getAction(Namespace ns)
{
	vector<Action> actions;
	
	for (auto i: nodes)
	{
		try
		{
			actions.push_back(i->getAction(ns));
		}
		catch (Err err)
		{
			err->log();
		}
	}
	
	Action out=listAction(actions);
	return out;
}

Type AstExpression::getReturnType(Namespace ns)
{
	return getAction(ns)->getReturnType();
}

Action AstExpression::getAction(Namespace ns)
{
	if (!action)
	{
		resolveAction(ns);
	}
	
	if (action)
		return action;
	else
		return voidAction;
}

void AstExpression::resolveAction(Namespace ns)
{
	if (!leftIn)
		throw PineconeError("AstExpression not given left input", INTERNAL_ERROR, token);
	
	if (!rightIn)
		throw PineconeError("AstExpression not given right input", INTERNAL_ERROR, token);
	
	Action leftAction=leftIn->getAction(ns);
	Action rightAction=rightIn->getAction(ns);
	
	try
	{
		Action out=ns->getActionForTokenWithInput(token, leftAction, rightAction);
	}
	catch (IdNotFoundError)
	{
		PineconeError
	}
	
}
