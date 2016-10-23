#pragma once

#include <string>
#include "ElementData.h"
#include "Type.h"
#include "Action.h"
#include "ActionTable.h"

using std::string;

class Element
{
public:
	
	Element(const Element&)=delete;
	
	Element(ElementData dataIn)
	{
		data=dataIn;
	}
	
	virtual ~Element()=default;
	
	virtual ActionPtr resolveActions(ActionTablePtr table)=0;
	
	virtual string getReadableName()
	{
		return data.toString();
	}
	
	ElementData getData() {return data;}
	
	virtual ElementData::Type getElemType()=0;
	
	//virtual void* execute()=0;
	
	virtual void printToString(string& in, int depth=-1);
	string printToBoxedString(string name);
	
protected:
	
	void printIndentationToString(string& in, int depth);
	
	ElementData data;
};

typedef std::shared_ptr<Element> ElementPtr;

