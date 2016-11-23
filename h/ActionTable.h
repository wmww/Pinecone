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
	
	Action addConverter(Action action, Type type)
	{
		vector<Type> types;
		types.push_back(type);
		return addConverter(action, types);
	}
	
	Action addConverter(Action action, vector<Type>& types);
	
	Action makeBranchAction(Token token, Action left, Action right);
	
	void addAction(Action in, string nameIn);
	void addAction(Action in, Operator op);
	
	Type getType(string name);
	
	void addType(Type typeIn);
	void addType(TypeData::PrimitiveType typeIn, string nameIn);
	void addType(vector<Type> typesIn, string nameIn);
	
	StackFrame* getStackFrame() {return stackFrame;}
	
	string toString();
	
	void clear();
	
private:
	void addActionsToList(vector<Action>& in, string& text);
	void addActionsToList(vector<Action>& in, Operator op);
	Action makeBranchAction(vector<Action>& matches, Action left, Action right);
	void getAllConvertersForType(vector<Action>& convertersOut, Type type);
	
	shared_ptr<ActionTable> parent;
	list<Action> converters;
	list<Action> actions;
	list<Action> operators;
	list<Type> types;
	
	StackFrame* stackFrame;
};

typedef shared_ptr<ActionTable> ActionTablePtr;
