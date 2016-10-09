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
	
	void resolveIdentifiers(IdentifierTable& table);
	
	ElementData::Type getElemType() {return ElementData::IDENTIFIER;}
	
	Type getReturnType();
	
	Identifier * getIdentifier() {return ptr;}
	void setIdentifier(Identifier * in) {ptr=in;}
	
private:
	Identifier * ptr=nullptr;
	IdentifierElement(ElementData dataIn): Element(dataIn) {}
	//IdentifierElement(ElementData data, Identifier * ptrIn): Element(data) {ptr=ptrIn;}
};


