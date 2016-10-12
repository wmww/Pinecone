#pragma once

#include "Element.h"
#include "Type.h"
#include "IdentifierTable.h"

class IdentifierElement: public Element
{
public:
	
	//table can be null, in which case it just won't look up anything
	static IdentifierElement * makeNew(ElementData data);
	
	string getReadableName();
	
	void resolveIdentifiers(IdentifierTable& table) {resolveIdentifiers(table, Type(Type::VOID), Type(Type::VOID));}
	
	void resolveIdentifiers(IdentifierTable& table, Type leftType, Type rightType);
	
	ElementData::Type getElemType() {return ElementData::IDENTIFIER;}
	
	Type getReturnType();
	
	//Identifier * getIdentifier() {return ptr;}
	//void setIdentifier(Identifier * in) {ptr=in;}
	
	DataElem * execute();
	
	DataElem * execute(DataElem * left, DataElem * right);
	
private:
	//Identifier * ptr=nullptr;
	Action * action=nullptr; //the index of the action in the identifier
	IdentifierElement(ElementData dataIn): Element(dataIn) {}
	//IdentifierElement(ElementData data, Identifier * ptrIn): Element(data) {ptr=ptrIn;}
};


