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

class VoidAction: public Action
{
public:
	VoidAction(): Action(Void, Void, Void, "Void")
	{
		setDescription("Void");
	}
	
	void* execute(void* inLeft, void* inRight)
	{
		return nullptr;
	}
};

extern ActionPtr voidAction;

//an action for getting a variable, will delete the data element in destructor
class VarGetAction: public Action
{
public:
	
	VarGetAction(size_t in, Type typeIn, string textIn):
		Action(typeIn, Void, Void, textIn)
	{
		offset=in;
		
		setDescription("get '" + textIn + "'");
	}
	
	void* execute(void* inLeft, void* inRight);
	
private:
	
	size_t offset;
};

//an action for setting a variable, will NOT delete the data element in destructor
class VarSetAction: public Action
{
public:
	
	VarSetAction(size_t in, Type typeIn, string textIn):
		Action(typeIn, Void, typeIn, textIn)
	{
		offset=in;
		
		setDescription("set '" + textIn + "'");
	}
	
	void* execute(void* left, void* right);
	
private:
	
	size_t offset;
};

class LiteralGetAction: public Action
{
public:
	
	LiteralGetAction(void* in, Type typeIn, string textIn):
		Action(typeIn, Void, Void, textIn)
	{
		data=malloc(returnType->getSize());
		memcpy(data, in, returnType->getSize());
		
		setDescription(textIn);// + " (" + typeIn.toString() + " literal)");
	}
	
	~LiteralGetAction()
	{
		free(data);
	}
	
	void* execute(void* inLeft, void* inRight)
	{
		void* out=malloc(returnType->getSize());
		memcpy(out, data, returnType->getSize());
		return out;
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
		setDescription(text);// + " (lambda action)");
	}
	
	void* execute(void* inLeft, void* inRight)
	{
		return lambda(inLeft, inRight);
	}
	
private:
	function<void*(void*,void*)> lambda;
};

