#pragma once

#include "Element.h"

#include <list>
#include <vector>
using std::list;
using std::vector;

#include "OperatorElement.h"

#include "ActionTable.h"

class TupleElement: public Element
{
public:
	
	TupleElement(ElementData data): Element(data) {}
	~TupleElement() {elems.clear();}
	
	string getReadableName() {return "tuple";}
	
	ActionPtr resolveActions(ActionTablePtr table);
	
	void printToString(string& in, int depth);
	using Element::printToString;
	
	void appendElement(ElementPtr in);
	
	ElementData::Type getElemType() {return ElementData::TUPLE;}
	
private:
	
	list<ElementPtr> elems;
};

