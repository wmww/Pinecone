#pragma once

//#include "Identifier.h"
#include "Type.h"
#include "Action.h"
#include "ElementData.h"
//#include "OperatorElement.h"
//#include "StackFrame.h"

#include <list>

using std::list;

class StackFrame;

class ActionTable
{
public:
	//should only be called within this class except for creating the root
	ActionTable(shared_ptr<ActionTable> parentIn);
	ActionTable(StackFrame * stackFrameIn);
	
	~ActionTable() {clear();}
	
	ActionPtr makeBranchAction(ElementData data, ActionPtr left, ActionPtr right);
	ActionPtr makeBranchAction(ElementData data, OperatorType opType, ActionPtr left, ActionPtr right);
	
	void addAction(ActionPtr in);
	void addAction(ActionPtr in, OperatorType opType);
	
	Type getType(string name);
	
	void addType(Type typeIn);
	void addType(TypeBase::PrimitiveType typeIn, string nameIn);
	void addType(vector<Type> typesIn, string nameIn);
	
	StackFrame* getStackFrame() {return stackFrame;}
	
	string toString();
	
	void clear();
	
private:
	void addActionsToList(vector<ActionPtr>& in, string& text);
	void addActionsToList(vector<ActionPtr>& in, OperatorType opType);
	ActionPtr makeBranchAction(vector<ActionPtr>& matches, ActionPtr left, ActionPtr right);
	void getAllConvertersForType(vector<ActionPtr>& convertersOut, Type type);
	
	shared_ptr<ActionTable> parent;
	list<ActionPtr> converters;
	list<ActionPtr> actions;
	list<ActionPtr> operators[OP_TYPE_OVERRIDEABLE_LAST];
	list<Type> types;
	
	StackFrame* stackFrame;
};

typedef shared_ptr<ActionTable> ActionTablePtr;
