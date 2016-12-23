#include "../h/AstNode.h"
#include "../h/ErrorHandler.h"
#include "../h/msclStringFuncs.h"
#include "../h/AllOperators.h"

//AstNode astVoid=AstNode(new AstVoid);

extern StackFrame stdLibStackFrame;
extern Namespace stdLibNamespace;

/// List

string AstList::getString()
{
	string out;
	
	out+="\n(";
	
	for (int i=0; i<int(nodes.size()); i++)
	{
		out+="\n"+indentString(nodes[i]->getString())+"\n";
	}
	
	out+=")";
	
	return out;
}

void AstList::inputWasSet()
{
	if (!inLeftType->isVoid() || !inRightType->isVoid())
	{
		throw PineconeError("AstList given non void input", INTERNAL_ERROR);
	}
	
	ns=ns->makeChild();
	
	for (int i=0; i<int(nodes.size()); i++)
	{
		nodes[i]->setInput(ns, Void, Void);
	}
}

void AstList::resolveReturnType()
{
	if (nodes.empty())
	{
		returnType=Void;
	}
	else
	{
		returnType=nodes.back()->getReturnType();
	}
}

void AstList::resolveAction()
{
	vector<Action> actions;
	
	for (int i=0; i<int(nodes.size()); i++)
	{
		try
		{
			actions.push_back(nodes[i]->getAction());
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
	
	if (!leftIn->isVoid())
	{
		out+=leftIn->getString();
		out+=" -> ";
	}
	
	out+=center->getString();
	
	if (!rightIn->isVoid())
	{
		out+=" <- ";
		out+=rightIn->getString();
	}
	
	out+=")";
	
	return out;
}

void AstExpression::inputWasSet()
{
	if (!inLeftType->isVoid() || !inRightType->isVoid())
	{
		throw PineconeError("AstExpression given non void input", INTERNAL_ERROR);
	}
	
	leftIn->setInput(ns, Void, Void);
	rightIn->setInput(ns, Void, Void);
	
	center->setInput(ns, leftIn->getReturnType(), rightIn->getReturnType());
}

void AstExpression::resolveAction()
{
	action=branchAction(leftIn->getAction(), center->getAction(), rightIn->getAction());
}


/// Token

string AstToken::getString()
{
	return token->getText();
}
	
void AstToken::resolveAction()
{
	try
	{
		action=ns->getActionForTokenWithInput(token, inLeftType, inRightType);
	}
	catch (IdNotFoundError err)
	{
		vector<Action> actions;
		ns->getActions(token->getText(), actions);
		
		if (actions.size()>0) // if there are actions with the requested name that didn't match the type
		{
			throw PineconeError("improper use or attempted redefinition of '"+token->getText()+"'", SOURCE_ERROR, token);
		}
		
		Type type=inRightType;
		
		if (type->getType()==TypeBase::METATYPE)
		{
			throw PineconeError("metatype handeling in "+FUNC+" not yet implemented", INTERNAL_ERROR, token);
		}
		else if (!type->isCreatable())
		{
			throw PineconeError("cannot create variable '"+token->getText()+"' of type "+type->getString(), SOURCE_ERROR, token);
		}
		
		ns->addVar(type, token->getText());
		action=ns->getActionForTokenWithInput(token, inLeftType, inRightType);
	}
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
		auto type=&subTypes[i];
		
		if (type->name)
		{
			out+=type->name->getText()+": ";
		}
		
		out+=type->type->getString();
		
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

