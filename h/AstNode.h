
#include "Token.h"
#include "Action.h"
#include "Namespace.h"

class AstNodeBase;

typedef shared_ptr<AstNodeBase> AstNode;

class AstNodeBase
{
public:
	virtual Type getReturnType(Namespace ns)=0;
	virtual Action getAction(Namespace ns)=0;
};

class AstVoid: public AstNodeBase
{
public:
	Type getReturnType(Namespace ns) {return Void;}
	Action getAction(Namespace ns) {return voidAction;}
};

extern AstNode astVoid;

class AstList: public AstNodeBase
{
public:
	
	void addNode(AstNode in) {nodes.push_back(in);}
	
	Type getReturnType(Namespace ns);
	Action getAction(Namespace ns);
	
private:
	vector<AstNode> nodes;
};

class AstExpression: public AstNodeBase
{
public:
	
	AstExpression(Token tokenIn)
	{
		leftIn=nullptr;
		token=tokenIn;
		rightIn=nullptr;
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
	
	Type getReturnType(Namespace ns);
	Action getAction(Namespace ns);
	
private:
	Token token;
	AstNode leftIn, rightIn;
};
