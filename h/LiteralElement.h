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
	
	Type getType() {return type;}
	
	string getReadableName() {return data.text + " (Int literal)";}
	
	ActionPtr resolveActions(ActionTablePtr table);
	
	ElementData::Type getElemType() {return ElementData::LITERAL;}
	
private:
	Type type;
	void* val;
};
