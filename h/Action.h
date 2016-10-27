#pragma once

#include "Type.h"
#include <functional>
#include <memory>

using std::shared_ptr;
using std::to_string;
using std::function;

class Action
{
public:
	
	Action(Type returnTypeIn, Type inLeftTypeIn, Type inRightTypeIn, string textIn);
	
	virtual ~Action() {}
	
	void setDescription(string in) {description=in;}
	
	string getText() {return text;}
	virtual string getDescription() {return description;}
	string toString();
	
	Type& getReturnType() {return returnType;}
	Type& getInLeftType() {return inLeftType;}
	Type& getInRightType() {return inRightType;}
	//void* execute(void* inLeft, void* inRight);
	virtual void* execute(void* inLeft, void* inRight)=0;
	
protected:
	
	string text;
	
	Type returnType;
	Type inLeftType;
	Type inRightType;
	string description;
	//virtual DataElem * privateExecute(DataElem * inLeft, DataElem * inRight)=0;
};

typedef shared_ptr<Action> ActionPtr;

extern ActionPtr voidAction;

class VoidAction: public Action
{
public:
	VoidAction(): Action(Void, Void, Void, "Void")
	{
		setDescription("Void Action");
	}
	
	void* execute(void* inLeft, void* inRight)
	{
		return nullptr;
	}
};

class LambdaAction: public Action
{
public:
	LambdaAction(Type returnTypeIn, function<void*(void*,void*)> lambdaIn, Type inLeftTypeIn, Type inRightTypeIn, string textIn);
	
	void* execute(void* inLeft, void* inRight);
	
private:
	function<void*(void*,void*)> lambda;
};

