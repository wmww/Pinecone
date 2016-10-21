#pragma once

#include "Element.h"

using std::to_string;

class LiteralElement: public Element
{
public:
	LiteralElement(void* valIn, Type typeIn, ElementData dataIn): Element(dataIn)
	{
		type=typeIn;
		val=type->cloneVoidPtr(valIn);
	}
	
	static ElementPtr makeNew(ElementData dataIn);
	
	Type getReturnType() {return type;}
	
	string getReadableName() {return data.text + " (Int literal)";}
	
	ActionPtr resolveActions(ActionTablePtr table);
	
	ElementData::Type getElemType() {return ElementData::LITERAL;}
	
private:
	Type type;
	void* val;
};

/*
class IntLiteral: public LiteralElement
{
public:
	string getReadableName() {return to_string(val) + " (Int literal)";}
	
	IntLiteral(ElementData dataIn, int in): LiteralElement(dataIn) {val=in;}
	
	Type getReturnType() {return Type::INT;}
	
	void* execute() {return new int(val);}
	
private:
	int val=0;
};

class DubLiteral: public LiteralElement
{
public:
	string getReadableName() {return to_string(val) + " (Dub literal)";}
	
	DubLiteral(ElementData dataIn, double in): LiteralElement(dataIn) {val=in;}
	
	Type getReturnType() {return Type::DUB;}
	
	void* execute() {return new double(val);}
	
private:
	double val=0;
};

class BoolLiteral: public LiteralElement
{
public:
	string getReadableName() {return to_string(val) + " (Bool literal)";}
	
	BoolLiteral(ElementData dataIn, bool in): LiteralElement(dataIn) {val=in;}
	
	Type getReturnType() {return Type::BOOL;}
	
	void* execute() {return new bool(val);}
	
private:
	bool val=0;
};


*/
