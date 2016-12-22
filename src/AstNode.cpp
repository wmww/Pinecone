#include "../h/AstNode.h"
#include "../h/ErrorHandler.h"
#include "../h/msclStringFuncs.h"
#include "../h/AllOperators.h"

AstNode astVoid=AstNode(new AstVoid);


/// List

string AstList::getString()
{
	string out;
	
	out+="{";
	
	for (auto i: nodes)
	{
		out+="\n"+indentString(i->getString())+"\n";
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

void AstList::resolveAction(Namespace ns)
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
	
	action=listAction(actions);
}


/// Expression

string AstExpression::getString()
{
	string out;
	
	if (leftIn!=astVoid || rightIn!=astVoid)
	{
		out+="(";
	}
	
	if (leftIn!=astVoid)
	{
		out+=leftIn->getString();
		out+=" -> ";
	}
	
	out+=token->getText();
	
	if (leftIn!=astVoid)
	{
		out+=" <- ";
		out+=rightIn->getString();
	}
	
	if (leftIn!=astVoid || rightIn!=astVoid)
	{
		out+=")";
	}
	
	return out;
}

void AstExpression::resolveAction(Namespace ns)
{
	Action leftAction=leftIn->getAction(ns);
	Action rightAction=rightIn->getAction(ns);
	
	try
	{
		action=ns->getActionForTokenWithInput(token, leftAction, rightAction);
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
		action=ns->getActionForTokenWithInput(token, leftAction, rightAction);
	}
}

