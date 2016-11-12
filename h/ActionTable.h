#pragma once

//#include "Identifier.h"
#include "Type.h"
#include "Action.h"
#include "Token.h"
#include "Operator.h"
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
	ActionTable(shared_ptr<ActionTable> parentIn, StackFrame * stackFrameIn);
	
	~ActionTable() {clear();}
	
	ActionPtr addConverter(ActionPtr action, Type type)
	{
		vector<Type> types;
		types.push_back(type);
		return addConverter(action, types);
	}
	
	ActionPtr addConverter(ActionPtr action, vector<Type>& types);
	
	ActionPtr makeBranchAction(Token token, ActionPtr left, ActionPtr right);
	
	void addAction(ActionPtr in);
	void addAction(ActionPtr in, Operator op);
	
	Type getType(string name);
	
	void addType(Type typeIn);
	void addType(TypeBase::PrimitiveType typeIn, string nameIn);
	void addType(vector<Type> typesIn, string nameIn);
	
	StackFrame* getStackFrame() {return stackFrame;}
	
	string toString();
	
	void clear();
	
private:
	void addActionsToList(vector<ActionPtr>& in, string& text);
	void addActionsToList(vector<ActionPtr>& in, Operator op);
	ActionPtr makeBranchAction(vector<ActionPtr>& matches, ActionPtr left, ActionPtr right);
	void getAllConvertersForType(vector<ActionPtr>& convertersOut, Type type);
	
	shared_ptr<ActionTable> parent;
	list<ActionPtr> converters;
	list<ActionPtr> actions;
	list<ActionPtr> operators;
	list<Type> types;
	
	StackFrame* stackFrame;
};

typedef shared_ptr<ActionTable> ActionTablePtr;
