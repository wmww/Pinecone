#pragma once

#include <vector>
using std::vector;

#include "Type.h"
//#include "Action.h"

extern void* globalFramePtr;
extern void* stackPtr;

class StackFrame
{
public:
	void addMember(Type in);
	
	//	set the left and right input types (can only be done once)
	void setInput(Type left, Type right);
	
	size_t getSize() {return frameSize;}
	Type getLeftInType();
	Type getRightInType();
	size_t getLeftOffset();
	size_t getRightOffset();
	
private:
	
	//Action actions; //must always be functionAction
	vector<Type> members;
	size_t frameSize=0;
	bool inputSet=false;
	int leftInputIndex=-1;
	int rightInputIndex=-1;
	size_t leftInputOffset;
	size_t rightInputOffset;
};
