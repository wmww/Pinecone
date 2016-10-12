#pragma once

#include "Type.h"
#include "DataElem.h"
#include "functional"

class Action
{
public:
	
	Action(Type returnTypeIn, Type inLeftTypeIn, Type inRightTypeIn, std::function<DataElem *(DataElem *, DataElem *)> lambdaIn, string nameIn)
	{
		name=nameIn;
		returnType=returnTypeIn;
		inLeftType=inLeftTypeIn;
		inRightType=inRightTypeIn;
		lambda=lambdaIn;
	}
	
	virtual ~Action() {}
	
	string getName() {return name;}
	
	Type& getReturnType() {return returnType;};
	Type& getInLeftType() {return inLeftType;};
	Type& getInRightType() {return inRightType;}
	DataElem * execute(DataElem * inLeft, DataElem * inRight);
	
protected:
	
	string name;
	
	Type returnType;
	Type inLeftType;
	Type inRightType;
	std::function<DataElem *(DataElem *, DataElem *)> lambda;
	//virtual DataElem * privateExecute(DataElem * inLeft, DataElem * inRight)=0;
};

//an action for getting a variable, will delete the data element in destructor
class VarGetAction: public Action
{
public:
	
	VarGetAction(DataElem * in, string nameIn):
		Action(in->getType(), Type(Type::VOID), Type(Type::VOID),
				[this](DataElem*, DataElem*)
				{
					return data->clone();
				}, "get '" + nameIn + "'")
	{
		data=in;
	}
	
	~VarGetAction() {delete data;}
	
private:
	
	DataElem * data;
};

//an action for setting a variable, will NOT delete the data element in destructor
class VarSetAction: public Action
{
public:
	
	VarSetAction(DataElem * in, string nameIn):
		Action(Type(Type::VOID), Type(Type::VOID), in->getType(),
				[this](DataElem* left, DataElem* right)
				{
					data->setData(right->getData());
					return new VoidData();
				}, "set '" + nameIn + "'")
	{
		data=in;
	}
	
private:
	
	DataElem * data;
};

