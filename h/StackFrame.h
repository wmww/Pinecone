#pragma once

#include <vector>
using std::vector;

#include "Type.h"
//#include "Action.h"

extern void* stackPtr;

class StackFrame
{
public:
	void addMember(Type in);
	void addLeftInput(Type in);
	void addRightInput(Type in);
	
	size_t getSize() {return frameSize;}
	size_t getLeftOffset() {return leftInputOffset;}
	size_t getRightOffset() {return rightInputOffset;}
	
private:
	
	//Action actions; //must always be functionAction
	vector<Type> members;
	size_t frameSize=0;
	bool leftInputAdded=false;
	bool rightInputAdded=false;
	size_t leftInputOffset=0;
	size_t rightInputOffset=0;
};
