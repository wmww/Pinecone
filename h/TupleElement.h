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
	
	TupleElement(ElementData dataIn, ElementList& parentList);
	ElementList(ElementData dataIn, StackFrame& frame);
	
	~ElementList() {clear();}
	
	string getReadableName() {return "tuple";}
	
	ActionPtr resolveActions(ActionTablePtr table);
	
	void printToString(string& in, int depth);
	using Element::printToString;
	
	void appendElement(ElementPtr in);
	
	ActionTablePtr getActionTable() {return table;}
	
	ElementData::Type getElemType() {return ElementData::TUPLE;}
	
	void* execute();
	
	void clear();
	
private:
	
	list<ElementPtr> elems;
};

