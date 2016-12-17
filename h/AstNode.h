#pragma once

#include "Token.h"
#include "Action.h"
#include "Namespace.h"

class AstNodeBase;

typedef shared_ptr<AstNodeBase> AstNode;

class AstNodeBase
{
public:
	
	virtual string getString()=0;
	
	virtual Type getReturnType(Namespace ns)=0;
	virtual Action getAction(Namespace ns)=0;
};

class AstVoid: public AstNodeBase
{
public:
	string getString() {return "void";}
	Type getReturnType(Namespace ns) {return Void;}
	Action getAction(Namespace ns) {return voidAction;}
};

extern AstNode astVoid;

class AstList: public AstNodeBase
{
public:
	
	void addNode(AstNode in) {nodes.push_back(in);}
	
	string getString();
	
	Type getReturnType(Namespace ns);
	Action getAction(Namespace ns);
	
private:
	vector<AstNode> nodes;
};

class AstExpression: public AstNodeBase
{
public:
	
	AstNode make(AstNode leftInIn, Token tokenIn, AstNode rightInIn)
	{
		AstExpression* node=new AstExpression;
		
		node->leftIn=leftInIn;
		node->token=tokenIn;
		node->rightIn=rightInIn;
		
		return AstNode(node);
	}
	
	/*
	AstExpression(AstNode leftInIn, Token tokenIn, AstNode rightInIn)
	{
		leftIn=leftInIn;
		token=tokenIn;
		rightIn=rightInIn;
	}
	*/
	
	void setLeftIn(AstNode in) {leftIn=in;}
	void setRightIn(AstNode in) {rightIn=in;}
	//void setToken(Token in) {token=in;}
	
	string getString();
	
	Type getReturnType(Namespace ns);
	Action getAction(Namespace ns);
	
	void resolveAction(Namespace ns);
	
private:
	
	AstExpression() {}
	
	Action action=nullptr;
	Token token=nullptr;
	AstNode leftIn=nullptr, rightIn=nullptr;
};
