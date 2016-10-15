#pragma once

#include "Type.h"
#include "functional"

using std::to_string;
using std::function;

class Action
{
public:
	
	Action(Type returnTypeIn, Type inLeftTypeIn, Type inRightTypeIn, string textIn)
	{
		text=textIn;
		returnType=returnTypeIn;
		inLeftType=inLeftTypeIn;
		inRightType=inRightTypeIn;
	}
	
	virtual ~Action() {}
	
	void setLambda(std::function<void*(void*, void*)> lambdaIn) {lambda=lambdaIn;}
	void setDescription(string in) {description=in;}
	
	string getText() {return text;}
	string getDescription() {return description;}
	
	Type& getReturnType() {return returnType;};
	Type& getInLeftType() {return inLeftType;};
	Type& getInRightType() {return inRightType;}
	//void* execute(void* inLeft, void* inRight);
	virtual void* execute(void* inLeft, void* inRight)=0;
	
protected:
	
	string text;
	
	Type returnType;
	Type inLeftType;
	Type inRightType;
	string description;
	std::function<void*(void*, void*)> lambda;
	//virtual DataElem * privateExecute(DataElem * inLeft, DataElem * inRight)=0;
};

//an action for getting a variable, will delete the data element in destructor
class VarGetAction: public Action
{
public:
	
	VarGetAction(void* in, Type typeIn, string textIn):
		Action(typeIn, Type(Type::VOID), Type(Type::VOID), textIn)
	{
		data=in;
		
		setDescription("get '" + textIn + "'");
	}
	
	void* execute(void* inLeft, void* inRight)
	{
		return returnType.cloneVoidPtr(data);
	}
	
	~VarGetAction() {returnType.deleteVoidPtr(data);}
	
private:
	
	void* data;
};

//an action for setting a variable, will NOT delete the data element in destructor
class VarSetAction: public Action
{
public:
	
	VarSetAction(void* in, Type typeIn, string textIn):
		Action(typeIn, Type(Type::VOID), typeIn, textIn)
	{
		data=in;
		
		setDescription("set '" + textIn + "'");
	}
	
	void* execute(void* left, void* right)
	{
		returnType.setVoidPtr(data, right);
		return returnType.cloneVoidPtr(data);
	}
	
private:
	
	void* data;
};

class LambdaAction: public Action
{
public:
	LambdaAction(Type returnTypeIn, function<void*(void*,void*)> lambdaIn, Type inLeftTypeIn, Type inRightTypeIn, string textIn)
		:Action(returnTypeIn, inLeftTypeIn, inRightTypeIn, textIn)
	{
		lambda=lambdaIn;
		setDescription(text + " (lambda action)");
	}
	
	void* execute(void* inLeft, void* inRight)
	{
		return lambda(inLeft, inRight);
	}
	
private:
	function<void*(void*,void*)> lambda;
};

