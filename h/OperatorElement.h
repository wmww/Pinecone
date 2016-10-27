#pragma once

#include "Element.h"

class OperatorElement: public Element
{
public:
	
	OperatorElement(ElementData dataIn, Operator opIn): Element(dataIn) {op=opIn;}
	
	string getReadableName();
	
	ActionPtr resolveActions(ActionTablePtr table);
	
	void printToString(string& in, int depth=0);
	//using Element::printToString;
	
	static ElementPtr makeNew(ElementData dataIn);
	
	void setLeftInput(ElementPtr in) {leftInput=in;}
	void setRightInput(ElementPtr in) {rightInput=in;}
	
	ElementData::Type getElemType() {return ElementData::OPERATOR;}
	
	Operator getOp() {return op;}
	
	void* execute();
	
private:
	
	Operator op;
	
	ElementPtr leftInput=nullptr;
	ElementPtr rightInput=nullptr;
};



