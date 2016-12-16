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
		catch (PineconeError err)
		{
			err.log();
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
	
	Action out;
	
	try
	{
		out=ns->getActionForTokenWithInput(token, leftAction, rightAction);
	}
	catch (IdNotFoundError err)
	{
		vector<Action> actions;
		ns->getActions(token->getText(), actions);
		
		if (actions.size()>0) // if there are actions with the requested name that didn't match the type
		{
			throw PineconeError("improper use or attempted redefinition of '"+token->getText()+"'", SOURCE_ERROR, token);
			
			/*error.log("available overloads:", JSYK, token);
			for (auto i: actions)
			{
				error.log("\t"+i->toString(), JSYK, token);
			}
			error.log("attempted use:", JSYK, token);
			error.log("\t"+leftIn->getReturnType()->getString()+"."+rightIn->getReturnType()->getString(), JSYK, token);
			return voidAction;*/
		}
		
		Type type=leftIn->getReturnType(ns);
		
		if (type->getType()==TypeBase::METATYPE)
		{
			throw PineconeError("metatype handeling in "+FUNC+" not yet implemented", INTERNAL_ERROR, token);
		}
		else if (!type->isCreatable())
		{
			throw PineconeError("cannot create variable '"+token->getText()+"' of type "+type->getString(), SOURCE_ERROR, token);
		}
		
		ns->addVar(type, token->getText());
		out=ns->getActionForTokenWithInput(token, leftAction, rightAction);
	}
	
	action=out;
}
