#pragma once

#include "Type.h"
#include "Action.h"
#include "Token.h"
#include "Operator.h"
#include "ErrorHandler.h"
#include "AstNode.h"

#include <unordered_map>
using std::unordered_map;

#include <vector>
using std::vector;

class StackFrame;

class NamespaceData;
typedef shared_ptr<NamespaceData> Namespace;

///	has a bunch of hash tables for all the identifiers in a single scope
//	contains a pointer to a stack frame but there may be many namespaces (due to many scopes) in a single stack frame
//		for example, a function has a single stack frame but has several if blocks, each with its own IdTable
//		each IdTable also has a pointer to its parent, which should only be null for the global standard library
//	contains a pointer to its parent namespace, which can be null if it is the root
//	can be created with one of the 'make' functions, not by directly instantiating it
class NamespaceData: public std::enable_shared_from_this<NamespaceData>
{
public:
	
	template<typename KEY>
	class ActionMap
	{
	public:
		void add(KEY key, AstNode node);
		void add(KEY key, Action action);
		void get(KEY key, vector<Action>& out);
		
	private:
		unordered_map<KEY, vector<AstNode>> nodes;
		unordered_map<KEY, vector<Action>> actions;
	};
	
	/// creation functions
	
	//	makes a namespace with no parents and a new stack frame
	static Namespace makeRootNamespace();
	
	//	makes a child namespace with the same stack frame as this one
	Namespace makeChild();
	
	//	makes a child namespace with a new stack frame
	Namespace makeChildAndFrame(string nameIn);
	
	
	///	getters
	
	//	returns a string with the complete contents of this namespace
	string getString();
	
	//	retuens a string with this namespace and all it's parents nicely formatted
	string getStringWithParents();
	
	Namespace getParent() {return parent;}
	shared_ptr<StackFrame> getStackFrame() {return stackFrame;}
	
	
	///	adding elements
	
	//	adds the input vars to the stack frame, will be called 'left' and 'right'
	void setInput(Type left, Type right);
	
	//	add a get and set action for a variable also adds its data to the stack frame
	void addVar(Type type, string name);
	
	//	used for adding generic actions AND converters (will autodetect if converter or not)
	void addAction(Action action, string id);
	
	// add an operator
	void addOperator(Action action, Operator op);
	
	//	add a type, throws an internal error if that type is already in the namespace, so check before trying to add
	void addType(Type type, string id);
	
	//	used for adding generic actions AND converters (will autodetect if converter or not)
	void addAction(AstNode node, string id);
	
	// add an operator
	void addOperator(AstNode node, Operator op);
	
	///	getting elements
	
	//	recursivly searches up looking for a type of the given name
	//	returns UnknownType if it cant find the requested type
	Type getType(string name);
	
	//	returns a branch action that is the action given in token with the left and right inputs
	//	returns voidAction if it can't find a good match
	Action getActionForTokenWithInput(Token token, Type left, Type right, bool dynamic);
	Action getActionForTokenWithInput(Token token, Action left, Action right, bool dynamic);
	
	//	returns the given action with a conversion to the given type put over top of it if needed
	Action getActionConvertedToType(Action actionIn, Type outType);
	
	//	addes all the matching actions in this and in all parents to out
	void getActions(string text, vector<Action>& out, bool dynamic);
	void getActions(Operator, vector<Action>& out);
	
	//Action addConverter(Action action, Type type);
	//Action addConverter(Action action, vector<Type>& types);
	
	//Action makeBranchAction(Token token, Action left, Action right);
	
	//void addAction(Action in, string nameIn);
	//void addAction(Action in, Operator op);
	
	//Type getType(string name);
	
	//void addType(Type typeIn);
	//void addType(TypeData::PrimitiveType typeIn, string nameIn);
	//void addType(vector<Type> typesIn, string nameIn);
	
private:
	
	//	the only constructor, is private so use a make function instead
	NamespaceData(Namespace parentIn, shared_ptr<StackFrame> stackFrameIn, string nameIn="");
	
	//void addActionsToList(vector<Action>& in, string& text);
	//void addActionsToList(vector<Action>& in, Operator op);
	//Action makeBranchAction(vector<Action>& matches, Action left, Action right);
	//void getAllConvertersForType(vector<Action>& convertersOut, Type type);
	
	//template<typename T, typename U>
	//void addToMap(T key, U val, unordered_map<T, vector<U>>& hashMap);
	
	//template<typename T, typename U>
	//void getValuesFromMap(T key, vector<U>& out, unordered_map<T, vector<U>>& hashMap);
	
	//void getConvertersToType(Type typeIn, vector<Action>& out);
	
	//	if there is exactly 1 action in the vector with the correct input types, returns it otherwise returns null
	//	NOTE: can return null
	Action findActionWithInput(vector<Action>& actionsIn, Type leftInType, Type rightInType);
	
	//	the name of this namespace, for debugging purposes only
	string myName;
	
	//	this namespaces parent
	shared_ptr<NamespaceData> parent;
	
	//	the stack frame this namespace uses
	//		does not have a reference back because there can be many namespaces in one stack frame
	shared_ptr<StackFrame> stackFrame;
	
	//	has the id type of every identifier in this namespace
	//unordered_map<string, vector<IdType>> allIds;
	
	ActionMap<string> dynamicActions;
	
	//	contains all generic actions in this namespace, and all converters (but not operators)
	ActionMap<string> actions;
	
	//	contains all converters (aka constructors), is a subset of actions
	//ActionMap<Type> converters;
	
	//	contains all operators
	ActionMap<Operator> operators;
	
	//	contains all types in this namespace
	ActionMap<string> types;
};

class IdNotFoundError
{
public:
	IdNotFoundError(string idIn, bool foundMatchIn, Namespace nsIn)
	{
		id=idIn;
		foundMatch=foundMatchIn;
		ns=nsIn;
	}
	
	string getId() {return id;}
	Namespace getNamespace() {return ns;}
	
	PineconeError toPineconeError(Token token)
	{
		string msg;
		
		if (foundMatch)
		{
			msg="proper overload for '"+id+"' not found";
		}
		else
		{
			msg="unknown identifier '"+id+"'";
		}
		
		return PineconeError(msg, SOURCE_ERROR, token);
	}
	
private:
	string id;
	bool foundMatch;
	Namespace ns;
};
