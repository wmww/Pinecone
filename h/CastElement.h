#pragma once

/*

#include "Element.h"
#include "Type.h"

class CastElement: public Element
{
public:
	
	CastElement(ElementData dataIn, Type typeIn, ElementPtr elemIn): Element(dataIn) {type=typeIn; input=elemIn;}
	
	~CastElement() {if (input) delete input;}
	
	string getReadableName() {return string() + "cast to " + type.toString();}
	
	ActionPtr resolveActions(ActionTable& table) {if (input) input->resolveActions(table);}
	
	void printToString(string& in, int depth=0);
	
	ElementData::Type getElemType() {return ElementData::CAST;}
	
	Type getReturnType() {return type;}
	
	void* execute();
	
private:
	
	//void* castToType(void* ptr, Type typeIn, Type typeOut);
	
	Type type;
	
	ElementPtr input;
};

*/

