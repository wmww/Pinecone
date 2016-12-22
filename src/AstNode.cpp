#include "../h/AstNode.h"
#include "../h/ErrorHandler.h"
#include "../h/msclStringFuncs.h"
#include "../h/AllOperators.h"

AstNode astVoid=AstNode(new AstVoid);


/// List

string AstList::getString()
{
	string out;
	
	out+="\n(";
	
	for (auto i: nodes)
	{
		out+="\n"+indentString(i->getString())+"\n";
	}
	
	out+=")";
	
	return out;
}

void AstList::resolveReturnType()
{
	if (nodes.empty())
	{
		returnType=Void;
	}
	else
	{
		returnType=nodes.back()->getReturnType(ns);
	}
}

void AstList::resolveAction()
{
	vector<Action> actions;
	
	for (auto i: nodes)
	{
		try
		{
			actions.push_back(i->getAction());
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
	
	out+="(";
	
	if (leftIn!=astVoid)
	{
		out+=leftIn->getString();
		out+=" -> ";
	}
	
	out+=center->getString();
	
	if (rightIn!=astVoid)
	{
		out+=" <- ";
		out+=rightIn->getString();
	}
	
	out+=")";
	
	return out;
}

void AstExpression::resolveAction()
{
	/*
	Action leftAction=leftIn->getAction();
	Action rightAction=rightIn->getAction();
	
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
	*/
	
	action=voidAction;
}


/// Token

string AstToken::getString()
{
	return token->getText();
}
	
void AstToken::resolveAction()
{
	action=voidAction;
}


/// TokenType

string AstTokenType::getString()
{
	return "{"+token->getText()+"}";
}
	
void AstTokenType::resolveAction()
{
	action=voidAction;
}


/// TupleType

string AstTupleType::getString()
{
	string out;
	
	out+="{";
	
	for (int i=0; i<int(subTypes.size()); i++)
	{
		auto type=subTypes[i];
		
		if (type.name)
		{
			out+=type.name->getText()+": ";
		}
		
		out+=type.type->getString();
		
		if (i<int(subTypes.size())-1)
		{
			out+=", ";
		}
	}
	
	out+="}";
	
	return out;
}

void AstTupleType::resolveAction()
{
	action=voidAction;
}

