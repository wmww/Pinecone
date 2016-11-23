#pragma once

#include <vector>
using std::vector;

#include "Type.h"
#include "Action.h"

extern unsigned char * stackPtr;

class StackFrame
{
public:
	StackFrame(){}
	
	void clear();
	
	void execute();
	
	void addMember(Type in) {members.push_back(in); frameSize+=in->getSize();}
	
	size_t getSize() {return frameSize;}
	
private:
	
	Action actions; //must always be functionAction
	vector<Type> members;
	size_t frameSize=0;
};
