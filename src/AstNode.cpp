#include "../h/AstNode.h"
#include "../h/Namespace.h"
#include "../h/ErrorHandler.h"
#include "../h/msclStringFuncs.h"
#include "../h/AllOperators.h"

//AstNode astVoid=AstNode(new AstVoid);

extern StackFrame stdLibStackFrame;
extern Namespace stdLibNamespace;

Action resolveLiteral(Token token);

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
	if (!inLeftType->isVoid() || !inRightType->isVoid())
	{
		throw PineconeError("AstList given non void input", INTERNAL_ERROR);
	}
	
	ns=ns->makeChild();
	
	for (int i=0; i<int(nodes.size()); i++)
	{
		nodes[i]->setInput(ns, dynamic, Void, Void);
		nodes[i]->dealWithConstants();
	}
	
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

void AstExpression::resolveAction()
{
	if (!inLeftType->isVoid() || !inRightType->isVoid())
	{
		throw PineconeError("AstExpression given non void input", INTERNAL_ERROR);
	}
	
	leftIn->setInput(ns, dynamic, Void, Void);
	rightIn->setInput(ns, dynamic, Void, Void);
	
	center->setInput(ns, dynamic, leftIn->getReturnType(), rightIn->getReturnType());
	
	//error.log("resolveAction called for "+getString(), JSYK);
	
	action=branchAction(leftIn->getAction(), center->getAction(), rightIn->getAction());
}


/// Const Expression

string AstConstExpression::getString()
{
	string out;
	
	out+="(";
	
	out+=center->getString();
	
	if (!rightIn->isVoid())
	{
		out+=" <<== ";
		out+=rightIn->getString();
	}
	
	out+=")";
	
	return out;
}

void AstConstExpression::resolveConstant()
{
	if (!inLeftType->isVoid() || !inRightType->isVoid())
	{
		throw PineconeError("AstExpression given non void input", INTERNAL_ERROR);
	}
	
	//leftIn->setInput(ns, Void, Void);
	rightIn->setInput(ns, false, Void, Void);
	
	//error.log("resolveAction called for "+getString(), JSYK);
	
	Action rightAction=rightIn->getAction();
	
	void * val=rightAction->execute(nullptr, nullptr);
	
	//center->setInput(ns, false, Void, rightIn->getReturnType());
	
	Action valAction=constGetAction(val, rightAction->getReturnType(), "const expression");
	
	ns->addAction(valAction, center->token->getText());
}


/// Operation with input

string AstOpWithInput::getString()
{
	string out;
	
	if (!leftIn.empty())
	{
		out+="(";
		
		for (int i=0; i<int(leftIn.size()); i++)
		{
			if (i>0)
				out+="	|";
				
			out+="\n"+indentString(leftIn[i]->getString())+"\n";
		}
		
		out+=") -> ";
	}
	
	out+=token->getText();
	
	if (!rightIn.empty())
	{
		out+=" -> (";
		
		for (int i=0; i<int(rightIn.size()); i++)
		{
			if (i>0)
				out+="	|";
				
			out+="\n"+indentString(rightIn[i]->getString())+"\n";
		}
		
		out+=")";
	}
	
	return out;
}

void AstOpWithInput::resolveAction()
{
	if (token->getOp()==ops->ifOp)
	{
		for (int i=0; i<int(leftIn.size()); i++)
			leftIn[i]->setInput(ns, dynamic, Void, Void);
			
		for (int i=0; i<int(rightIn.size()); i++)
			rightIn[i]->setInput(ns, dynamic, Void, Void);
			
		if (leftIn.empty())
		{
			throw PineconeError("'?' must have a conditional to its left", SOURCE_ERROR, token);
		}
		else if (leftIn.size()!=1)
		{
			throw PineconeError("'?' can only have one conditional to its left", SOURCE_ERROR, token);
		}
		
		Action condition=leftIn[0]->getAction();
		
		if (rightIn.empty())
		{
			throw PineconeError("'?' must have a statement to its right", SOURCE_ERROR, token);
		}
		else if (rightIn.size()<=2)
		{
			Action a;
			
			try
			{
				a=rightIn[0]->getAction();
			}
			catch (PineconeError err)
			{
				err.log();
				a=voidAction;
			}
			
			if (rightIn.size()==1)
			{
				action=ifAction(condition, a);
			}
			else
			{
				Action e;
				
				try
				{
					e=rightIn[1]->getAction();
				}
				catch (PineconeError err)
				{
					err.log();
					e=voidAction;
				}
				
				action=ifElseAction(condition, a, e);
			}
		}
		else
		{
			throw PineconeError("'?' can only have 1 or 2 '|' seporated expressions to its right", SOURCE_ERROR, token);
		}
	}
	else if (token->getOp()==ops->loop)
	{
		if (leftIn.size()==3)
			ns=ns->makeChild();
			
		for (int i=0; i<int(leftIn.size()); i++)
			leftIn[i]->setInput(ns, dynamic, Void, Void);
			
		for (int i=0; i<int(rightIn.size()); i++)
			rightIn[i]->setInput(ns, dynamic, Void, Void);
			
		Action initAction=nullptr, conditionAction, endAction, bodyAction;
		
		if (rightIn.size()>1)
		{
			throw PineconeError("'@' followed by multiple expressions", SOURCE_ERROR, token);
		}
		
		if (leftIn.size()==0)
		{
			throw PineconeError("condition needed before '@'", SOURCE_ERROR, token);
		}
		else if (leftIn.size()==1)
		{
			conditionAction=leftIn[0]->getAction();
			endAction=voidAction;
		}
		else if (leftIn.size()==2)
		{
			conditionAction=leftIn[0]->getAction();
			endAction=leftIn[1]->getAction();
		}
		else if (leftIn.size()==3)
		{
			initAction=leftIn[0]->getAction();
			conditionAction=leftIn[1]->getAction();
			endAction=leftIn[2]->getAction();
		}
		else
		{
			throw PineconeError("chain of length "+to_string(leftIn.size())+"preceding '@', it should be length 1-3", SOURCE_ERROR, token);
		}
		
		if (rightIn.empty())
		{
			bodyAction=voidAction;
		}
		else
		{
			try
			{
				bodyAction=rightIn[0]->getAction();
			}
			catch (PineconeError err)
			{
				err.log();
				bodyAction=voidAction;
			}
		}
		
		action=loopAction(conditionAction, endAction, bodyAction);
		
		if (initAction)
		{
			vector<Action> actions;
			actions.push_back(initAction);
			actions.push_back(action);
			action=listAction(actions);
		}
	}
	else
	{
		throw PineconeError("AstOpWithInput made with bad token '"+token->getText()+"'", INTERNAL_ERROR, token);
	}
}


/// Token

string AstToken::getString()
{
	return token->getText();
}

void AstToken::resolveAction()
{
	//error.log("resolveAction called for token "+token->getText(), JSYK, token);
	
	if (token->getType()==TokenData::IDENTIFIER || token->getType()==TokenData::OPERATOR)
	{
		if (token->getOp() && !token->getOp()->isOverloadable())
		{
			throw PineconeError("non overloadable operator in AstToken, it should have been removed and processed by the parser", INTERNAL_ERROR, token);
		}
		
		if (inLeftType->getType()==TypeBase::TUPLE && inLeftType->getSubType(token->getText()).type!=Unknown)
		{
			if (inRightType->isVoid())
			{
				action=getElemFromTupleAction(inLeftType, token->getText());
			}
			else
			{
				throw PineconeError("sorry, Pinecone does not yet support mutating tuples", SOURCE_ERROR, token);
			}
			
			return;
		}
		
		try
		{
			//error.log("looking for "+token->getText()+" in\n"+ns->getStringWithParents(), JSYK, token);
			action=ns->getActionForTokenWithInput(token, inLeftType, inRightType, dynamic);
		}
		catch (IdNotFoundError err)
		{
			vector<Action> actions;
			ns->getActions(token->getText(), actions, dynamic);
			
			if (actions.size()>0) // if there are actions with the requested name that didn't match the type
			{
				throw PineconeError("improper use or attempted redefinition of '"+token->getText()+"'", SOURCE_ERROR, token);
			}
			
			if (token->getType()==TokenData::OPERATOR)
			{
				throw PineconeError("unknown overload for operator '"+token->getText()+"'", SOURCE_ERROR, token);
			}
			
			Type type=inRightType;
			
			if (type->getType()==TypeBase::METATYPE)
			{
				throw PineconeError("metatype handeling in "+FUNC+" not yet implemented", INTERNAL_ERROR, token);
			}
			else if (type->isVoid())
			{
				throw PineconeError("unknown identifier '"+token->getText()+"' (var can not be made bc right in type is "+type->getString()+")", SOURCE_ERROR, token);
			}
			else if (!type->isCreatable())
			{
				throw PineconeError("cannot create variable '"+token->getText()+"' of type "+type->getString(), SOURCE_ERROR, token);
			}
			
			ns->addVar(type, token->getText());
			
			try
			{
				action=ns->getActionForTokenWithInput(token, inLeftType, inRightType, dynamic);
			}
			catch (IdNotFoundError err)
			{
				throw err.toPineconeError(token);
			}
		}
	}
	else if (token->getType()==TokenData::LITERAL)
	{
		if (!inLeftType->isVoid() || !inRightType->isVoid())
		{
			throw PineconeError("a literal can not be given an input", SOURCE_ERROR, token);
		}
		
		action=resolveLiteral(token);
	}
	else
	{
		throw PineconeError("AstToken givin invalid token '"+token->getText()+"' of type "+TokenData::typeToString(token->getType()), INTERNAL_ERROR, token);
	}
}


/// Tuple

string AstTuple::getString()
{
	string out;
	
	out+="(";
	
	for (int i=0; i<int(nodes.size()); i++)
	{
		if (i)
			out+=", ";
			
		out+=nodes[i]->getString();
	}
	
	out+=")";
	
	return out;
}

void AstTuple::resolveAction()
{
	vector<Action> actions;
	
	for (int i=0; i<int(nodes.size()); i++)
	{
		nodes[i]->setInput(ns, dynamic, Void, Void);
		actions.push_back(nodes[i]->getAction());
	}
	
	action=makeTupleAction(actions);
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

