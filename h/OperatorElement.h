#pragma once

#include "Element.h"

class OperatorElement: public Element
{
public:
	
	///steps to adding a new operator
	/**
		1. add to the enum					below
		2. add it to toString				OperatorElement.cpp				so the enum will return a useful string
		3. add it to makeNew				OperatorElement.cpp				so the right source code character will be matched with the enum
		4. add it to populateCharVectors	PineconeParser.cpp				so it can be detected in the first place
	**/
	
	OperatorElement(ElementData dataIn, OperatorType typeIn): Element(dataIn) {opType=typeIn;}
	
	static string toString(OperatorType);
	
	string getReadableName() {return toString(opType) + " (" + getReturnType().toString() + ")";}
	
	Type getReturnType() {return action?action->getReturnType():Type();}
	
	ActionPtr resolveActions(ActionTablePtr table);
	
	void printToString(string& in, int depth=0);
	//using Element::printToString;
	
	static ElementPtr makeNew(ElementData dataIn);
	
	void setLeftInput(ElementPtr in) {leftInput=in;}
	void setRightInput(ElementPtr in) {rightInput=in;}
	
	ElementData::Type getElemType() {return ElementData::OPERATOR;}
	
	OperatorType getType() {return opType;}
	
	void* execute();
	
private:
	
	OperatorType opType;
	
	ActionPtr action;
	
	//Type returnType;
	
	ElementPtr leftInput=nullptr;
	ElementPtr rightInput=nullptr;
};



