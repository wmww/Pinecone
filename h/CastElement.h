#pragma once

#include "Element.h"
#include "Type.h"

class CastElement: public Element
{
public:
	
	CastElement(ElementData dataIn, Type typeIn, Element * elemIn): Element(dataIn) {type=typeIn; input=elemIn;}
	
	~CastElement() {if (input) delete input;}
	
	string getReadableName() {return string() + "cast to " + type.toString();}
	
	void resolveIdentifiers(IdentifierTable& table) {if (input) input->resolveIdentifiers(table);}
	
	void printToString(string& in, int depth=0);
	
	ElementData::Type getElemType() {return ElementData::CAST;}
	
	Type getReturnType() {return type;}
	
	DataElem * execute();
	
private:
	
	DataElem * castToType(DataElem * ptr, Type type);
	
	Type type;
	
	Element * input;
};



