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
	enum OpType
	{
		DOT,
		PLUS,
		MINUS,
		COLON,
		OPEN,
		CLOSE
	};
	
	OperatorElement(ElementData dataIn, OpType typeIn): Element(dataIn) {opType=typeIn;}
	
	~OperatorElement();
	
	static string toString(OpType);
	
	string getReadableName() {return toString(opType) + " (" + returnType.toString() + ")";}
	
	Type getReturnType() {return returnType;}
	
	void resolveIdentifiers(ActionTable& table);
	
	void printToString(string& in, int depth=0);
	//using Element::printToString;
	
	static OperatorElement * makeNew(ElementData dataIn);
	
	void setLeftInput(Element * in) {leftInput=in;}
	void setRightInput(Element * in) {rightInput=in;}
	
	ElementData::Type getElemType() {return ElementData::OPERATOR;}
	
	OpType getType() {return opType;}
	
	void* execute();
	
private:
	
	OpType opType;
	
	Type returnType;
	
	Element * leftInput=nullptr;
	Element * rightInput=nullptr;
};



