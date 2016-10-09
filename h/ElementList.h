#pragma once

#include "Element.h"
#include <list>
#include <vector>

using std::list;
using std::vector;

#include "OperatorElement.h"

#include "IdentifierTable.h"

class ElementList: public Element
{
public:
	
	ElementList(ElementData dataIn, ElementList * parent=nullptr);
	
	~ElementList() {clear();}
	
	string getReadableName() {return "element list";}
	
	Type getReturnType();	
	
	void structureByOperators();
	
	void resolveIdentifiers();
	void resolveIdentifiers(IdentifierTable& table) {resolveIdentifiers();}
	
	void absorbForOperators(vector<OperatorElement::OpType> operators);
	
	void printToString(string& in, int depth);
	using Element::printToString;
	
	void appendElement(Element * in);
	
	ElementData::Type getElemType() {return ElementData::SCOPE;}
	
	void clear();
	
private:
	
	list<Element *> elems;
	
	IdentifierTable table;
};

