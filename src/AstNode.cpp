#include "../h/AstNode.h"
#include "../h/ErrorHandler.h"
#include "../h/msclStringFuncs.h"

AstNode astVoid=AstNode(new AstVoid);

string AstList::getString()
{
	string out;
	
	out+="{";
	
	for (auto i: nodes)
	{
		out+=indentString(i->getString()+"\n")+"\n";
	}
	
	out+="}";
	
	return out;
}

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

string AstExpression::getString()
{
	string out;
	
	out+="(";
	out+=leftIn->getString();
	out+=") -> ";
	out+=token->getText();
	out+=" <- (";
	out+=rightIn->getString();
	out+=")";
	
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

