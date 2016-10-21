#pragma once

#include "Element.h"
#include "Type.h"
#include "ActionTable.h"

class IdentifierElement: public Element
{
public:
	
	//table can be null, in which case it just won't look up anything
	static ElementPtr makeNew(ElementData data);
	
	string getReadableName();
	
	ActionPtr resolveActions(ActionTablePtr table) {return resolveActions(table, Void, Void);}
	
	ActionPtr resolveActions(ActionTablePtr table, Type leftType, Type rightType);
	
	ElementData::Type getElemType() {return ElementData::IDENTIFIER;}
	
	//Type getReturnType();
	
	//Identifier * getIdentifier() {return ptr;}
	//void setIdentifier(Identifier * in) {ptr=in;}
	
	void* execute();
	
	void* execute(void* left, void* right);
	
private:
	
	//Type returnType;
	
	//Identifier * ptr=nullptr;
	//ActionPtr action=nullptr; //the index of the action in the identifier
	IdentifierElement(ElementData dataIn): Element(dataIn) {}
	//IdentifierElement(ElementData data, Identifier * ptrIn): Element(data) {ptr=ptrIn;}
};


