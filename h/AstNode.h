#pragma once

#include "Token.h"
#include "Action.h"
#include "Namespace.h"

#include <vector>
using std::vector;

class AstNodeBase;

typedef shared_ptr<AstNodeBase> AstNode;

AstNode astNodeFromTokens(const vector<Token>&, int start, int end);

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
	
	shared_ptr<AstVoid> make() {return shared_ptr<AstVoid>(new AstVoid);}
	
	string getString() {return "void";}
	Type getReturnType(Namespace ns) {return Void;}
	Action getAction(Namespace ns) {return voidAction;}
};

extern AstNode astVoid;

class AstList: public AstNodeBase
{
public:
	
	//	make a new instance of this type of node
	shared_ptr<AstList> make() {return shared_ptr<AstList>(new AstList);}
	
	//	add a sub node to the end of the list
	void addNode(AstNode in) {nodes.push_back(in);}
	
	string getString();
	
	Type getReturnType(Namespace ns);
	Action getAction(Namespace ns);
	
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
	
	shared_ptr<AstExpression> make(AstNode leftInIn, Token tokenIn, AstNode rightInIn)
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
	
	Type getReturnType(Namespace ns);
	Action getAction(Namespace ns);
	
	void resolveAction(Namespace ns);
	
private:
	
	AstExpression() {}
	
	//	the action this node maps to, starts as nullptr and is resolved 
	Action action=nullptr;
	Token token=nullptr;
	AstNode leftIn=nullptr, rightIn=nullptr;
};
