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
	
	//Identifier * makeIdentifier(string name) {return table.makeIdentifier(name);}
	
	Type getReturnType();	
	
	void structureByOperators();
	
	void resolveIdentifiers();
	void resolveIdentifiers(ActionTable& table) {resolveIdentifiers();}
	
	void absorbForOperators(vector<OperatorElement::OpType> operators);
	
	void printToString(string& in, int depth);
	using Element::printToString;
	
	void appendElement(Element * in);
	
	void addAction(Action * in) {table.addAction(in);}
	
	ElementData::Type getElemType() {return ElementData::SCOPE;}
	
	void* execute();
	
	void clear();
	
private:
	
	list<Element *> elems;
	
	ActionTable table;
};

