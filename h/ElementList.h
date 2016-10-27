#pragma once

#include "Element.h"
#include <list>
#include <vector>

using std::list;
using std::vector;
using std::unique_ptr;

#include "OperatorElement.h"

#include "ActionTable.h"

class ElementList: public Element
{
public:
	
	ElementList(ElementData dataIn, ElementList& parentList);
	ElementList(ElementData dataIn, StackFrame& frame);
	
	~ElementList() {clear();}
	
	string getReadableName() {return "element list";}
	
	//Identifier * makeIdentifier(string name) {return table.makeIdentifier(name);}
	
	void structureByOperators();
	
	ActionPtr resolveActions();
	ActionPtr resolveActions(ActionTablePtr table) {return resolveActions();}
	
	void printToString(string& in, int depth);
	using Element::printToString;
	
	void appendElement(ElementPtr in);
	
	ActionTablePtr getActionTable() {return table;}
	
	ElementData::Type getElemType() {return ElementData::SCOPE;}
	
	void* execute();
	
	void clear();
	
private:
	
	void absorbePeren();
	
	void absorbeDotInDub();
	
	void absorbForOperators(vector<Operator> operators, bool absorbLeft, bool absorbRight, bool backwords);
	
	list<ElementPtr> elems;
	
	ActionTablePtr table;
};

