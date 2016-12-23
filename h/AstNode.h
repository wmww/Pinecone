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
	
	void setInput(Namespace nsIn, Type left, Type right)
	{
		if (inputHasBeenSet)
		{
			throw PineconeError("tried to set input on an AST node '"+getString()+"' more then once", INTERNAL_ERROR);
		}
		
		inputHasBeenSet=true;
		
		ns=nsIn;
		inLeftType=left;
		inRightType=right;
		
		inputWasSet();
	}
	
	virtual bool isVoid() {return false;}
	
	virtual string getString()=0;
	
	Type getReturnType()
	{
		if (!returnType)
		{
			if (!ns)
			{
				throw PineconeError("tried to get return type from AST node when input had not been set", INTERNAL_ERROR);
			}
			
			resolveReturnType();
		}
		
		return returnType;
	}
	
	Action getAction()
	{
		if (!action)
		{
			if (!ns)
			{
				throw PineconeError("tried to get action from AST node when input had not been set", INTERNAL_ERROR);
			}
			
			resolveAction();
		}
		
		return action;
	}
	
protected:
	
	AstNodeBase() {}
	
	virtual void inputWasSet() {}
	
	virtual void resolveReturnType()
	{
		returnType=getAction()->getReturnType();
	}
	
	virtual void resolveAction()=0;
	
	Type inLeftType=nullptr, inRightType=nullptr;
	Action action=nullptr;
	Type returnType=nullptr;
	Namespace ns=nullptr;
	bool inputHasBeenSet=false;
};

class AstVoid: public AstNodeBase
{
public:
	
	static shared_ptr<AstVoid> make() {return shared_ptr<AstVoid>(new AstVoid);}
	
	bool isVoid() {return true;}
	
	string getString() {return "void node";}
	
	void inputWasSet()
	{
		if (!inLeftType->isVoid() || !inRightType->isVoid())
		{
			throw PineconeError("AstVoid given non void input", INTERNAL_ERROR);
		}
	}
	
	void resolveReturnType() {returnType=Void;}
	void resolveAction() {action=voidAction;}
};
//extern AstNode astVoid;

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
	
	void inputWasSet();
	
	void resolveReturnType();
	
	void resolveAction();
	
private:
	
	//	the list of sub nodes
	vector<AstNode> nodes;
};

class AstExpression: public AstNodeBase
{
public:
	
	static shared_ptr<AstExpression> make(AstNode leftInIn, AstNode centerIn, AstNode rightInIn)
	{
		shared_ptr<AstExpression> node(new AstExpression);
		
		node->leftIn=leftInIn;
		node->center=centerIn;
		node->rightIn=rightInIn;
		
		return node;
	}
	
	string getString();
	
	void inputWasSet();
	
	void resolveAction();
	
private:
	
	AstNode leftIn=nullptr, center=nullptr, rightIn=nullptr;
};

class AstToken: public AstNodeBase
{
public:
	
	static shared_ptr<AstToken> make(Token tokenIn)
	{
		shared_ptr<AstToken> node(new AstToken);
		node->token=tokenIn;
		return node;
	}
	
	string getString();
	
	void resolveAction();
	
private:
	
	Token token=nullptr;
};

class AstType: public AstNodeBase
{
	
};

class AstVoidType: public AstType
{
public:
	
	static shared_ptr<AstVoidType> make()
	{
		shared_ptr<AstVoidType> node(new AstVoidType);
		return node;
	}
	
	string getString() {return "{}";}
	
	void resolveAction() {action=voidAction;}
	
private:
};

class AstTokenType: public AstType
{
public:
	
	static shared_ptr<AstTokenType> make(Token tokenIn)
	{
		shared_ptr<AstTokenType> node(new AstTokenType);
		node->token=tokenIn;
		return node;
	}
	
	string getString();
	
	void resolveAction();
	
private:
	
	Token token=nullptr;
};

class AstTupleType: public AstType
{
public:
	
	struct NamedType
	{
		Token name; // can be null
		shared_ptr<AstType> type;
	};
	
	static shared_ptr<AstTupleType> make(const vector<NamedType>& in)
	{
		shared_ptr<AstTupleType> node(new AstTupleType);
		node->subTypes=in;
		return node;
	}
	
	string getString();
	
	void resolveAction();
	
private:
	
	vector<NamedType> subTypes;
};





