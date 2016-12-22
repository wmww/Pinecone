#pragma once

#include "Token.h"
#include "Action.h"
#include "Namespace.h"

#include <vector>
using std::vector;

class AstNodeBase;

typedef shared_ptr<AstNodeBase> AstNode;

AstNode astNodeFromTokens(const vector<Token>&);

class AstNodeBase
{
public:
	
	virtual string getString()=0;
	
	virtual Type getReturnType(Namespace ns)
	{
		return getAction(ns)->getReturnType();
	}
	
	Action getAction(Namespace ns)
	{
		if (!action)
		{
			resolveAction(ns);
		}
		
		return action;
	}
	
protected:
	
	virtual void resolveAction(Namespace ns)=0;
	
	Action action=nullptr;
};

class AstVoid: public AstNodeBase
{
public:
	
	static shared_ptr<AstVoid> make() {return shared_ptr<AstVoid>(new AstVoid);}
	
	string getString() {return "void node";}
	Type getReturnType(Namespace ns) {return Void;}
	void resolveAction(Namespace ns) {action=voidAction;}
};

extern AstNode astVoid;

class AstList: public AstNodeBase
{
public:
	
	//	make a new instance of this type of node
	static shared_ptr<AstList> make(const vector<AstNode>& in)
	{
		shared_ptr<AstList> node(new AstList);
		node->nodes=in;
		return node;
	}
	
	string getString();
	
	Type getReturnType(Namespace ns);
	
	void resolveAction(Namespace ns);
	
private:
	
	//	the list of sub nodes
	vector<AstNode> nodes;
};

class AstExpression: public AstNodeBase
{
public:
	
	//	make a new instance of this type of node
	/*
	AstNode make(AstNode leftInIn, Token tokenIn, AstNode rightInIn)
	{
		AstExpression* node=new AstExpression;
		
		node->leftIn=leftInIn;
		node->token=tokenIn;
		node->rightIn=rightInIn;
		
		return AstNode(node);
	}
	*/
	
	static shared_ptr<AstExpression> make(AstNode leftInIn, Token tokenIn, AstNode rightInIn)
	{
		shared_ptr<AstExpression> node(new AstExpression);
		
		node->leftIn=leftInIn;
		node->token=tokenIn;
		node->rightIn=rightInIn;
		
		return node;
	}
	
	
	//	set the inputs of this node
	//void setLeftIn(AstNode in) {leftIn=in; action=nullptr;}
	//void setRightIn(AstNode in) {rightIn=in;}
	//void setToken(Token in) {token=in;}
	
	string getString();
	
	void resolveAction(Namespace ns);
	
private:
	
	AstExpression() {}
	
	Token token=nullptr;
	AstNode leftIn=nullptr, rightIn=nullptr;
};


