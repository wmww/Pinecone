#include "../h/AstNode.h"
#include "../h/Namespace.h"
#include "../h/ErrorHandler.h"
#include "../h/msclStringFuncs.h"
#include "../h/utils/stringDrawing.h"
#include "../h/AllOperators.h"

//AstNode astVoid=AstNode(new AstVoid);

extern StackFrame stdLibStackFrame;
extern Namespace globalNamespace;

Action resolveLiteral(Token token);

void AstNodeBase::copyToNode(AstNodeBase* other, bool copyCache)
{
	other->inLeftType=inLeftType;
	other->inRightType=inRightType;
	other->nameHint=nameHint;
	
	if (copyCache)
	{
		other->action=action;
		other->returnType=returnType;
		other->dynamic=dynamic;
		other->ns=ns;
		other->inputHasBeenSet=inputHasBeenSet;
	}
}

/// List

string AstList::getString()
{
	vector<string> data;
	
	for (int i=0; i<int(nodes.size()); i++)
	{
		data.push_back(nodes[i]->getString());
	}
	
	return str::makeList(data);
}

/*void AstList::resolveReturnType()
{
	if (nodes.empty())
	{
		returnType=Void;
	}
	else
	{
		nodes.back()->setInput(ns, dynamic, Void, Void);
		returnType=nodes.back()->getReturnType();
	}
}*/

void AstList::resolveAction()
{
	if (!inLeftType->isVoid() || !inRightType->isVoid())
	{
		throw PineconeError("AstList given non void input", INTERNAL_ERROR, getToken());
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
			Action action=nodes[i]->getAction();
			
			if (i!=(int)nodes.size()-1)
				action=ns->wrapInDestroyer(action);
			
			actions.push_back(action);
		}
		catch (PineconeError err)
		{
			err.log();
		}
	}
	
	action=listAction(actions, *ns->getDestroyerActions());
}


/// Function body

string AstFuncBody::getString()
{
	vector<string> data;
	
	data.push_back("function");
	
	vector<string> types={leftTypeNode->getString(), rightTypeNode->getString(), returnTypeNode->getString()};
	
	data.push_back(str::makeList(types));
	
	data.push_back(bodyNode->getString());
	
	return str::makeList(data);
}

AstNode AstFuncBody::makeCopyWithSpecificTypes(Type leftInType, Type rightInType)
{
	Type leftWhatevType=leftTypeNode->getReturnType()->getSubType();
	Type rightWhatevType=rightTypeNode->getReturnType()->getSubType();
	
	if (!leftInType->matches(leftWhatevType) || !rightInType->matches(rightWhatevType))
	{
		return nullptr;
	}
	
	AstNode actualLeftTypeNode;
	AstNode actualRightTypeNode;
	
	if (leftWhatevType->isWhatev())
	{
		actualLeftTypeNode=AstTypeType::make(leftWhatevType->actuallyIs(leftInType));
	}
	else
	{
		actualLeftTypeNode=leftTypeNode->makeCopy(false);
	}
	
	if (rightWhatevType->isWhatev())
	{
		actualRightTypeNode=AstTypeType::make(rightWhatevType->actuallyIs(rightInType));
	}
	else
	{
		actualRightTypeNode=rightTypeNode->makeCopy(false);
	}
	
	AstNode out=make(move(actualLeftTypeNode), move(actualRightTypeNode), returnTypeNode->makeCopy(false), bodyNode->makeCopy(false));
	out->nameHint=nameHint;
	out->setInput(ns, dynamic, Void, Void);
	return out;
}

void AstFuncBody::resolveAction()
{
	setTypesInput();
	Namespace subNs=ns->makeChildAndFrame(nameHint.empty()?"unnamed function's namespace":nameHint+"'s namespace");
	subNs->setInput(leftTypeNode->getReturnType()->getSubType(), rightTypeNode->getReturnType()->getSubType());
	bodyNode->setInput(subNs, true, Void, Void);
	Type funcReturnType=returnTypeNode->getReturnType()->getSubType();
	if (funcReturnType->isWhatev())
	{
		funcReturnType=funcReturnType->actuallyIs(bodyNode->getReturnType());
	}
	action=functionAction(bodyNode->makeCopy(true), funcReturnType, subNs->getStackFrame());
}


/// Expression

string AstExpression::getString()
{
	string leftStr;
	if (!leftIn->isVoid())
	{
		leftStr=leftIn->getString();
	}
	
	string centerStr=center->getString();
	
	string rightStr;
	if (!rightIn->isVoid())
	{
		rightStr=rightIn->getString();
	}
	
	return str::makeRootUpBinaryTree(centerStr, "", "", leftStr, rightStr);
}

void AstExpression::resolveAction()
{
	if (!inLeftType->isVoid() || !inRightType->isVoid())
	{
		throw PineconeError("AstExpression given non void input", INTERNAL_ERROR, getToken());
	}
	
	if (rightIn->isType())
	{
		throw PineconeError("types must be declared as constants", SOURCE_ERROR, rightIn->getToken());
	}
	else if (center->isType() || center->isFunctionWithOutput() || leftIn->isType())
	{
		throw PineconeError("a function implementation got into an expression node somehow", INTERNAL_ERROR, center->getToken());
	}
	else
	{
		leftIn->setInput(ns, dynamic, Void, Void);
		rightIn->setInput(ns, dynamic, Void, Void);
		
		center->setInput(ns, dynamic, leftIn->getReturnType(), rightIn->getReturnType());
		
		//error.log("left: " + leftIn->getString(), JSYK);
		//error.log("center: " + center->getString(), JSYK);
		//error.log("right: " + rightIn->getString(), JSYK);
		//error.log("", JSYK);
		
		action=branchAction(leftIn->getAction(), center->getAction(), rightIn->getAction());
	}
	
	if (action->nameHint.empty())
		action->nameHint=nameHint;
}


/// Const Expression

string AstConstExpression::getString()
{
	string centerStr=center->getString();
	
	string rightStr;
	if (!rightIn->isVoid())
	{
		rightStr=rightIn->getString();
	}
	
	return str::makeRootUpBinaryTree(centerStr, "", "const", "", rightStr);
}

void AstConstExpression::resolveConstant()
{
	if (!inLeftType->isVoid() || !inRightType->isVoid())
	{
		throw PineconeError("AstConstExpression given non void input", INTERNAL_ERROR, getToken());
	}
	
	//leftIn->setInput(ns, Void, Void);
	rightIn->setInput(ns, false, Void, Void);
	
	//error.log("resolveAction called for "+getString(), JSYK);
	
	ns->addNode(move(rightIn->makeCopy(true)), center->token->getText());
	
	/*
	Action rightAction=rightIn->getAction();
	
	void * val=rightAction->execute(nullptr, nullptr);
	
	//center->setInput(ns, false, Void, rightIn->getReturnType());
	
	Action valAction=constGetAction(val, rightAction->getReturnType(), "const expression");
	
	ns->addAction(valAction, center->token->getText());*/
}


/// Operation with input

string AstOpWithInput::getString()
{
	string left;
	
	vector<string> data;
	
	for (int i=0; i<int(leftIn.size()); i++)
	{
		data.push_back(leftIn[i]->getString());
	}
	
	if (data.size()==1)
		left=data[0];
	else if (data.size()>1)
		left=str::makeList(data);
	
	data.clear();
	
	string right;
	
	for (int i=0; i<int(rightIn.size()); i++)
	{
		data.push_back(rightIn[i]->getString());
	}
	
	if (data.size()==1)
		right=data[0];
	else if (data.size()>1)
		right=str::makeList(data);
	
	return str::makeRootUpBinaryTree(str::putStringInTreeNodeBox(token->getText()), "", "", left, right);
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
		bool usesSubNS=false;
		
		if (leftIn.size()==3)
		{
			ns=ns->makeChild();
			usesSubNS=true;
		}
			
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
		
		vector<Action> actions;
		
		if (initAction)
			actions.push_back(ns->wrapInDestroyer(initAction));
		
		actions.push_back(ns->wrapInDestroyer(loopAction(conditionAction, endAction, bodyAction)));
		
		action=usesSubNS ?
				action=listAction(actions, *ns->getDestroyerActions())
			:
				action=listAction(actions, {})
		;
	}
	else if (token->getOp()==ops->andOp || token->getOp()==ops->orOp)
	{
		if (leftIn.size()>1 || rightIn.size()>1)
		{
			throw PineconeError("'"+token->getOp()->getText()+"' can not be sent '|' separated sequence", SOURCE_ERROR, getToken());
		}
		
		if (leftIn.size()!=1 || rightIn.size()!=1)
		{
			throw PineconeError("'"+token->getOp()->getText()+"' must be given a left and right input", SOURCE_ERROR, getToken());
		}
		
		leftIn[0]->setInput(ns, dynamic, Void, Void);
		rightIn[0]->setInput(ns, dynamic, Void, Void);
		
		Action leftAction=leftIn[0]->getAction();
		Action rightAction=rightIn[0]->getAction();
		
		if (leftAction->getReturnType()!=Bool)
		{
			throw PineconeError("'"+token->getOp()->getText()+"' can only be used with Bools", SOURCE_ERROR, leftIn[0]->getToken());
		}
		
		if (rightAction->getReturnType()!=Bool)
		{
			throw PineconeError("'"+token->getOp()->getText()+"' can only be used with Bools", SOURCE_ERROR, rightIn[0]->getToken());
		}
		
		if (token->getOp()==ops->andOp)
		{
			action=andAction(leftAction, rightAction);
		}
		else // (token->getOp()==ops->orOp)
		{
			action=orAction(leftAction, rightAction);
		}
	}
	else if (token->getOp()==ops->rightArrow)
	{
		throw PineconeError("AstOpWithInput::resolveAction called for token '"+token->getOp()->getText()+"', which it shouldn't have been", INTERNAL_ERROR, token);
	}
	else
	{
		throw PineconeError("AstOpWithInput made with bad token '"+token->getText()+"'", INTERNAL_ERROR, token);
	}
}

bool AstOpWithInput::isFunctionWithOutput()
{
	return token->getOp()==ops->rightArrow && leftIn.size()==1 && rightIn.size()==1;
}


/// Token

string AstToken::getString()
{
	return str::putStringInTreeNodeBox(token->getText());
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
		
		/*
		if (inLeftType->getType()==TypeBase::TUPLE && inLeftType->getSubType(token->getText()).type!=nullptr)
		{
			if (inRightType->isVoid())
			{
				action=getElemFromTupleAction(inLeftType, token->getText());
				return;
			}
			else
			{
				throw PineconeError("sorry, Pinecone does not yet support mutating tuples", SOURCE_ERROR, token);
			}
		}
		*/
		
		action=ns->getActionForTokenWithInput(token, inLeftType, inRightType, dynamic, true, token);
		
		/*
		try
		{
			//error.log("looking for "+token->getText()+" in\n"+ns->getStringWithParents(), JSYK, token);
			action=ns->getActionForTokenWithInput(token, inLeftType, inRightType, dynamic);
		}
		catch (IdNotFoundError err)
		{
			if (token->getType()==TokenData::OPERATOR)
			{
				throw PineconeError("unknown overload for "+inLeftType->getString()+" "+token->getText()+" "+inRightType->getString()+"'", SOURCE_ERROR, token);
			}
			else if (token->getType()==TokenData::IDENTIFIER)
			{
				vector<Action> actions;
				ns->getActions(token->getText(), actions, dynamic);
				
				if (actions.size()>0) // if there are actions with the requested name that didn't match the type
				{
					string posibleInputs="the following is the inputs it can take:";
					
					for (int i=0; i<(int)actions.size(); i++)
					{
						posibleInputs+="\n    ";
						posibleInputs+=actions[i]->getInLeftType()->getString()+" . "+actions[i]->getInRightType()->getString()+" -> "+actions[i]->getReturnType()->getString();
					}
					
					throw PineconeError(
						"'"+token->getText()+"' can not take input of type "+
						inLeftType->getString()+" . "+inRightType->getString()+"\n"+
						posibleInputs,
						SOURCE_ERROR, token);
				}
			}
			
			if (inRightType->getType()==TypeBase::METATYPE)
			{
				throw PineconeError("metatype handeling in "+FUNC+" not yet implemented", INTERNAL_ERROR, token);
			}
			else if (inRightType->isVoid())
			{
				//throw PineconeError("unknown identifier '"+token->getText()+"' (var can not be made bc right in type is "+type->getString()+")", SOURCE_ERROR, token);
				throw PineconeError("unknown identifier '"+token->getText()+"'", SOURCE_ERROR, token);
			}
			else if (!inRightType->isCreatable())
			{
				throw PineconeError("cannot create variable '"+token->getText()+"' of type {"+inRightType->getString()+"}", SOURCE_ERROR, token);
			}
			
			ns->addVar(inRightType, token->getText());
			
			try
			{
				action=ns->getActionForTokenWithInput(token, inLeftType, inRightType, dynamic);
			}
			catch (IdNotFoundError err)
			{
				throw err.toPineconeError(token);
			}
		}
		*/
	}
	else if (token->getType()==TokenData::LITERAL || token->getType()==TokenData::STRING_LITERAL)
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
	vector<string> data;
	
	for (int i=0; i<int(nodes.size()); i++)
	{
		data.push_back(nodes[i]->getString());
	}
	
	return str::makeList(data);
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

void AstTokenType::resolveReturnType()
{
	returnType=ns->getType(token->getText(), true, token)->getMeta();
}


/// TupleType

string AstTupleType::getString()
{
	string out;
	
	out+="AstTupleType{";
	
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

void AstTupleType::resolveReturnType()
{
	TupleTypeMaker maker;
	
	for (unsigned i=0; i<subTypes.size(); i++)
	{
		subTypes[i].type->setInput(ns, false, Void, Void);
		
		if (subTypes[i].name)
		{
			maker.add(subTypes[i].name->getText(), subTypes[i].type->getReturnType()->getSubType());
		}
		else
		{
			maker.add(subTypes[i].type->getReturnType()->getSubType());
		}
	}
	
	returnType=maker.get(true)->getMeta();
}


