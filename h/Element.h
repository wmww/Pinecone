#pragma once

#include <string>
#include "ElementData.h"
#include "Type.h"
#include "IdentifierTable.h"
#include "DataElem.h"

using std::string;

class Element
{
public:
	
	Element(ElementData dataIn)
	{
		data=dataIn;
	}
	
	virtual ~Element()=default;
	
	virtual void resolveIdentifiers(IdentifierTable& table) {}
	
	virtual string getReadableName()
	{
		return data.toString();
	}
	
	ElementData getData() {return data;}
	
	virtual ElementData::Type getElemType()=0;
	
	virtual Type getReturnType() {return Type::UNKNOWN;}
	
	virtual DataElem * execute()=0;
	
	virtual void printToString(string& in, int depth=-1);
	string printToBoxedString(string name);
	
protected:
	
	void printIndentationToString(string& in, int depth);
	
	ElementData data;
};

