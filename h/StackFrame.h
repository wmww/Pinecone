#pragma once

#include <vector>
using std::vector;

#include "Type.h"
#include "ElementList.h"

extern unsigned char * stackPtr;

class StackFrame
{
public:
	StackFrame(){}
	
	void clear();
	
	void appendElement(ElementPtr in);
	
	void resolve(bool printOutput);
	
	void execute();
	
	void addMember(Type in) {members.push_back(in); frameSize+=in->getSize();}
	
	size_t getSize() {return frameSize;}
	
	ElementList& getElementList() {return elements;}
	
private:
	
	ElementList elements={ElementData("", "UNKNOWN_FILE", 0, ElementData::SCOPE), *this};
	ActionPtr actions; //must always be functionAction
	vector<Type> members;
	size_t frameSize=0;
};
