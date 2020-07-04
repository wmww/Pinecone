#pragma once

#include "Token.h"
#include "Action.h"
#include "ErrorHandler.h"

class NamespaceData;
typedef shared_ptr<NamespaceData> Namespace;

#include <vector>
using std::vector;

class AstNodeBase;
typedef unique_ptr<AstNodeBase> AstNode;

AstNode astNodeFromTokens(const vector<Token>& tokens, int left, int right);

class AstNodeBase
{
public:

	virtual ~AstNodeBase() = default;
	
	void setInput(Namespace nsIn, bool dynamicIn, Type left, Type right)
	{
		if (inputHasBeenSet)
		{
			throw PineconeError("tried to set input on an AST node '"+getString()+"' more then once", INTERNAL_ERROR, getToken());
		}
		
		inputHasBeenSet=true;
		
		ns=nsIn;
		dynamic=dynamicIn;
		inLeftType=left;
		inRightType=right;
	}
	
	virtual bool isVoid() {return false;}
	virtual bool isType() {return false;}
	virtual bool isFunctionWithOutput() {return false;}
	
	/*
	Type getInLeftType()
	{
		if (!inputHasBeenSet)
			throw PineconeError("tried to get inLeftType before input was set", INTERNAL_ERROR, getToken());
		return inLeftType;
	}
	
	Type getInRightType()
	{
		if (!inputHasBeenSet)
			throw PineconeError("tried to get inRightType before input was set", INTERNAL_ERROR, getToken());
		return inRightType;
	}
	*/
	
	virtual string getString()=0;
	
	virtual AstNode makeCopy(bool copyCache)=0; // if copyCache is false, input and actions will not be copied
	
	virtual bool canBeWhatev() {return false;}
	
	// returns nullptr if it can't do it
	virtual AstNode makeCopyWithSpecificTypes(Type leftInType, Type rightInType) {return nullptr;}
	
	Type getReturnType()
	{
		if (!returnType)
		{
			if (!inputHasBeenSet)
			{
				throw PineconeError("tried to get return type from AST node when input had not been set", INTERNAL_ERROR, getToken());
			}
			
			resolveReturnType();
		}
		
		if (!returnType)
		{
			throw PineconeError("AST node "+getString()+"failed to supply a return type", INTERNAL_ERROR);
		}
		
		return returnType;
	}
	
	Action getAction()
	{
		if (!action)
		{
			if (!ns)
			{
				throw PineconeError("tried to get action from AST node when input had not been set", INTERNAL_ERROR, getToken());
			}
			
			resolveAction();
			
			if (!nameHint.empty())
				action->nameHint=nameHint;
		}
		
		return action;
	}
	
	void dealWithConstants()
	{
		if (!ns)
		{
			throw PineconeError("tried to deal with constants before input was set", INTERNAL_ERROR, getToken());
		}
		
		resolveConstant();
	}
	
	// primarily used for error throwing, can return null
	virtual Token getToken()=0;
	
	string nameHint="";
	virtual void nameHintSet() {}
	
protected:
	
	AstNodeBase() {}
	
	void copyToNode(AstNodeBase* other, bool copyCache);
	
	virtual void resolveReturnType()
	{
		returnType=getAction()->getReturnType();
	}
	
	virtual void resolveAction()=0;
	virtual void resolveConstant() {};
	
	Type inLeftType=nullptr, inRightType=nullptr;
	Action action=nullptr;
	Type returnType=nullptr;
	bool dynamic=false;
	Namespace ns=nullptr;
	bool inputHasBeenSet=false;
};

class AstVoid: public AstNodeBase
{
public:
	
	static unique_ptr<AstVoid> make() {return unique_ptr<AstVoid>(new AstVoid);}
	
	bool isVoid() {return true;}
	
	string getString() {return "void node";}
	
	AstNode makeCopy(bool copyCache)
	{
		auto out=new AstVoid;
		copyToNode(out, copyCache);
		return AstNode(out);
	}
	
	void resolveReturnType() {returnType=Void;}
	
	void resolveAction()
	{
		if (!inLeftType->isVoid() || !inRightType->isVoid())
		{
			throw PineconeError("AstVoid given non void input", INTERNAL_ERROR, getToken());
		}
		
		action=voidAction;
	}
	
	Token getToken() {return nullptr;}
};
//extern AstNode astVoid;

class AstList: public AstNodeBase
{
public:
	
	//	make a new instance of this type of node
	static unique_ptr<AstList> make(vector<AstNode>& in)
	{
		unique_ptr<AstList> node(new AstList);
		node->nodes=move(in);
		return node;
	}
	
	string getString();
	
	AstNode makeCopy(bool copyCache)
	{
		auto out=new AstList;
		copyToNode(out, copyCache);
		for (int i=0; i<(int)nodes.size(); i++)
		{
			out->nodes.push_back(nodes[i]->makeCopy(copyCache));
		}
		return AstNode(out);
	}
	
	//void resolveReturnType();
	
	void resolveAction();
	
	Token getToken() {return nodes.empty()?nullptr:nodes[0]->getToken();}
	
private:
	
	//	the list of sub nodes
	vector<AstNode> nodes;
};

class AstConstExpression;

class AstToken: public AstNodeBase
{
public:
	
	static unique_ptr<AstToken> make(Token tokenIn)
	{
		unique_ptr<AstToken> node(new AstToken);
		node->token=tokenIn;
		return node;
	}
	
	string getString();
	
	AstNode makeCopy(bool copyCache)
	{
		auto out=new AstToken;
		copyToNode(out, copyCache);
		out->token=token;
		return AstNode(out);
	}
	
	void resolveAction();
	
	Token getToken() {return token;}
	
private:
	
	friend AstConstExpression;
	
	Token token=nullptr;
};

class AstFuncBody: public AstNodeBase
{
public:
	
	static AstNode make(AstNode leftTypeIn, AstNode rightTypeIn, AstNode returnTypeIn, AstNode bodyIn)
	{
		auto node=new AstFuncBody();
		
		if (!leftTypeIn->isType() || !rightTypeIn->isType() || !returnTypeIn->isType())
		{
			throw PineconeError("AstFuncBody made with function input nodes that are not types", INTERNAL_ERROR);
		}
		
		node->leftTypeNode=move(leftTypeIn);
		node->rightTypeNode=move(rightTypeIn);
		node->returnTypeNode=move(returnTypeIn);
		node->bodyNode=move(bodyIn);
		
		return AstNode(node);
	}
	
	string getString();
	
	AstNode makeCopy(bool copyCache)
	{
		auto out=new AstFuncBody;
		copyToNode(out, copyCache);
		out->leftTypeNode=leftTypeNode->makeCopy(copyCache);
		out->rightTypeNode=rightTypeNode->makeCopy(copyCache);
		out->returnTypeNode=returnTypeNode->makeCopy(copyCache);
		out->bodyNode=bodyNode->makeCopy(copyCache);
		out->typesInputSet=typesInputSet;
		return AstNode(out);
	}
	
	void resolveAction();
	
	AstNode makeCopyWithSpecificTypes(Type leftInType, Type rightInType);
	
	Token getToken() {return bodyNode->getToken();}
	
	void setTypesInput()
	{
		if (!typesInputSet)
		{
			leftTypeNode->setInput(ns, false, Void, Void);
			rightTypeNode->setInput(ns, false, Void, Void);
			returnTypeNode->setInput(ns, false, Void, Void);
			typesInputSet=true;
		}
	}
	
	bool canBeWhatev()
	{
		setTypesInput();
		
		return
			leftTypeNode->getReturnType()->isWhatev() ||
			rightTypeNode->getReturnType()->isWhatev() ||
			returnTypeNode->getReturnType()->isWhatev();
	
	}
	
	AstNode leftTypeNode, rightTypeNode, returnTypeNode, bodyNode;
	bool typesInputSet=false;
};

class AstExpression: public AstNodeBase
{
public:
	
	static unique_ptr<AstExpression> make(AstNode leftInIn, AstNode centerIn, AstNode rightInIn)
	{
		unique_ptr<AstExpression> node(new AstExpression);
		
		node->leftIn=move(leftInIn);
		node->center=move(centerIn);
		node->rightIn=move(rightInIn);
		
		return node;
	}
	
	//bool isType() {return leftIn->isType() || rightIn->isType();}
	
	AstNode makeCopy(bool copyCache)
	{
		auto out=new AstExpression;
		copyToNode(out, copyCache);
		out->leftIn=leftIn->makeCopy(copyCache);
		out->center=center->makeCopy(copyCache);
		out->rightIn=rightIn->makeCopy(copyCache);
		return AstNode(out);
	}
	
	string getString();
	
	void resolveAction();
	
	Token getToken() {return center->getToken();}
	
	AstNode leftIn=nullptr, center=nullptr, rightIn=nullptr;
};

class AstConstExpression: public AstNodeBase
{
public:
	
	static unique_ptr<AstConstExpression> make(unique_ptr<AstToken> centerIn, AstNode rightInIn)
	{
		unique_ptr<AstConstExpression> node(new AstConstExpression);
		
		//node->leftIn=move(leftInIn);
		node->center=move(centerIn);
		node->rightIn=move(rightInIn);
		
		return node;
	}
	
	string getString();
	
	AstNode makeCopy(bool copyCache)
	{
		auto out=new AstConstExpression;
		copyToNode(out, copyCache);
		out->center=unique_ptr<AstToken>((AstToken*)center->makeCopy(copyCache).release());;
		out->rightIn=center->makeCopy(copyCache);
		return AstNode(out);
	}
	
	void resolveConstant();
	void resolveAction() {action=voidAction;};
	
	Token getToken() {return center->getToken();}
	
private:
	
	//AstNode leftIn=nullptr;
	unique_ptr<AstToken> center=nullptr;
	AstNode rightIn=nullptr;
};

class AstOpWithInput: public AstNodeBase
{
public:
	static unique_ptr<AstOpWithInput> make(vector<AstNode>& leftInIn, Token tokenIn, vector<AstNode>& rightInIn)
	{
		unique_ptr<AstOpWithInput> node(new AstOpWithInput);
		
		node->leftIn=move(leftInIn);
		node->token=tokenIn;
		node->rightIn=move(rightInIn);
		
		return node;
	}
	
	bool isFunctionWithOutput();
	
	string getString();
	
	AstNode makeCopy(bool copyCache)
	{
		auto out=new AstOpWithInput;
		copyToNode(out, copyCache);
		out->token=token;
		for (int i=0; i<(int)leftIn.size(); i++)
		{
			out->leftIn.push_back(leftIn[i]->makeCopy(copyCache));
		}
		for (int i=0; i<(int)rightIn.size(); i++)
		{
			out->rightIn.push_back(rightIn[i]->makeCopy(copyCache));
		}
		return AstNode(out);
	}
	
	void resolveAction();
	
	Token getToken() {return token;}
	
	Token token=nullptr;
	vector<AstNode> leftIn, rightIn;
};

class AstTuple: public AstNodeBase
{
public:
	
	//	make a new instance of this type of node
	static unique_ptr<AstTuple> make(vector<AstNode>& in)
	{
		unique_ptr<AstTuple> node(new AstTuple);
		node->nodes=move(in);
		return node;
	}
	
	string getString();
	
	AstNode makeCopy(bool copyCache)
	{
		auto out=new AstTuple;
		copyToNode(out, copyCache);
		for (int i=0; i<(int)nodes.size(); i++)
		{
			out->nodes.push_back(nodes[i]->makeCopy(copyCache));
		}
		return AstNode(out);
	}
	
	//void resolveReturnType();
	
	void resolveAction();
	
	Token getToken() {return nodes.empty()?nullptr:nodes[0]->getToken();}
	
private:
	
	vector<AstNode> nodes;
};

class AstType: public AstNodeBase
{
public:
	bool isType() {return true;}
	
	void resolveAction()
	{
		throw PineconeError("AstType::resolveAction called, which it shouldn't have been", INTERNAL_ERROR, getToken());
	}
};

class AstTypeType: public AstType
{
public:
	
	static unique_ptr<AstTypeType> make(Type typeIn)
	{
		unique_ptr<AstTypeType> node(new AstTypeType);
		node->returnTypeNotMeta=typeIn;
		return node;
	}
	
	string getString()
	{
		return returnType->getString();
	}
	
	AstNode makeCopy(bool copyCache)
	{
		auto out=new AstTypeType;
		copyToNode(out, copyCache);
		out->returnType=returnType;
		return AstNode(out);
	}
	
	void resolveReturnType()
	{
		returnType=returnTypeNotMeta->getMeta();
	}
	
	void nameHintSet()
	{
		if (!nameHint.empty() && returnTypeNotMeta->nameHint.empty())
			returnTypeNotMeta->nameHint=nameHint;
	}
	
	Token getToken() {return nullptr;}
	
private:
	
	Type returnTypeNotMeta;
};

class AstVoidType: public AstType
{
public:
	
	static unique_ptr<AstVoidType> make()
	{
		unique_ptr<AstVoidType> node(new AstVoidType);
		return node;
	}
	
	string getString() {return "{}";}
	
	AstNode makeCopy(bool copyCache)
	{
		auto out=new AstVoidType;
		copyToNode(out, copyCache);
		return AstNode(out);
	}
	
	void resolveReturnType()
	{
		returnType=Void->getMeta();
	}
	
	Token getToken() {return nullptr;}
	
private:
};

class AstTokenType: public AstType
{
public:
	
	static unique_ptr<AstTokenType> make(Token tokenIn)
	{
		unique_ptr<AstTokenType> node(new AstTokenType);
		node->token=tokenIn;
		return node;
	}
	
	string getString();
	
	AstNode makeCopy(bool copyCache)
	{
		auto out=new AstTokenType;
		copyToNode(out, copyCache);
		out->token=token;
		return AstNode(out);
	}
	
	void resolveReturnType();
	
	Token getToken() {return token;}
	
private:
	
	Token token=nullptr;
};

class AstTupleType: public AstType
{
public:
	
	struct NamedType
	{
		Token name; // can be null
		unique_ptr<AstType> type;
	};
	
	static unique_ptr<AstTupleType> make(vector<NamedType>& in)
	{
		unique_ptr<AstTupleType> node(new AstTupleType);
		node->subTypes=move(in);
		return node;
	}
	
	string getString();
	
	AstNode makeCopy(bool copyCache)
	{
		auto out=new AstTupleType;
		copyToNode(out, copyCache);
		for (int i=0; i<(int)subTypes.size(); i++)
		{
			out->subTypes.push_back({subTypes[i].name, unique_ptr<AstType>((AstType*)subTypes[i].type->makeCopy(copyCache).release())});
		}
		return AstNode(out);
	}
	
	void resolveReturnType();
	
	Token getToken() {return subTypes.empty()?nullptr:subTypes[0].name;}
	
private:
	
	vector<NamedType> subTypes;
};

class AstActionWrapper: public AstNodeBase
{
public:
	
	static unique_ptr<AstActionWrapper> make(Action actionIn) {
		auto out = unique_ptr<AstActionWrapper>(new AstActionWrapper);
		out->inLeftType=actionIn->getInLeftType();
		out->inRightType=actionIn->getInRightType();
		out->returnType=actionIn->getReturnType();
		out->action=actionIn;
		out->dynamic=true; // shouldn't matter
		out->ns=nullptr; // shouldn't matter
		out->inputHasBeenSet=true;
		return out;
	}
	
	string getString() {return "action wrapper node";}
	
	AstNode makeCopy(bool copyCache)
	{
		auto out=new AstActionWrapper;
		copyToNode(out, true);
		return AstNode(out);
	}
	
	void resolveAction()
	{
		throw PineconeError("AstActionWrapper::resolveAction called, which it shouldn't have been", INTERNAL_ERROR);
	}
	
	Token getToken() {return nullptr;}
};

class AstWhatevToActionFactory: public AstNodeBase
{
public:
	
	static AstNode make(function<Action(Type left, Type right)> lambda)
	{
		auto node=new AstWhatevToActionFactory();
		node->lambda=lambda;
		return AstNode(node);
	}
	
	string getString() {return "AstWhatevToActionFactory";}
	
	AstNode makeCopy(bool copyCache)
	{
		auto out=new AstWhatevToActionFactory;
		copyToNode(out, copyCache);
		out->lambda=lambda;
		return AstNode(out);
	}
	
	void resolveAction() {throw PineconeError("AstWhatevToActionFactory::resolveAction called, wich should never happen", INTERNAL_ERROR);}
	
	AstNode makeCopyWithSpecificTypes(Type leftInType, Type rightInType)
	{
		auto action=lambda(leftInType, rightInType);
		if (action)
			return AstActionWrapper::make(action);
		else
			return nullptr;
	}
	
	Token getToken() {return nullptr;}
	
	bool canBeWhatev() {return true;}
	
private:
	function<Action(Type leftInType, Type rightInType)> lambda;
};







