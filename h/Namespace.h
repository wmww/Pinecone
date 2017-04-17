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

/// has a bunch of hash tables for all the identifiers in a single scope
// contains a pointer to a stack frame but there may be many namespaces (due to many scopes) in a single stack frame
//  for example, a function has a single stack frame but has several if blocks, each with its own NamespaceData
//  each NamespaceData also has a pointer to its parent, which should only be null for the global standard library
// contains a pointer to its parent namespace, which can be null if it is the root
// can be created with one of the 'make' functions, not by directly instantiating it
class NamespaceData: public std::enable_shared_from_this<NamespaceData>
{
public:
	
	/// creation functions
	
	// makes a namespace with no parents and a new stack frame
	static Namespace makeRootNamespace();
	
	// makes a child namespace with the same stack frame as this one
	Namespace makeChild();
	
	// makes a child namespace with a new stack frame
	Namespace makeChildAndFrame(string nameIn);
	
	
	/// getters
	
	// returns a string with the complete contents of this namespace
	string getString();
	
	// retuens a string with this namespace and all it's parents nicely formatted
	string getStringWithParents();
	
	Namespace getParent() {return parent;}
	shared_ptr<StackFrame> getStackFrame() {return stackFrame;}
	
	
	/// adding elements
	
	// adds the input vars to the stack frame, will be called 'me' and 'in'
	void setInput(Type left, Type right);
	
	// add a get and set action for a variable also adds its data to the stack frame
	Action addVar(Type type, string name);
	
	// add a type, throws an internal error if that type is already in the namespace, so check before trying to add
	void addType(Type type, string id);
	
	// used for adding generic actions AND converters (will autodetect if converter or not)
	void addNode(AstNode node, string id);
	
	// add an operator
	void addNode(AstNode node, Operator op);
	
	
	/// getting elements
	
	// recursivly searches up looking for a type of the given name
	// returns UnknownType if it cant find the requested type
	Type getType(string name, bool throwSourceError);
	
	Action getActionForTokenWithInput(Token token, Type left, Type right, bool dynamic, bool throwSourceError);
	
	// returns a branch action that is the action given in token with the left and right inputs
	// returns voidAction if it can't find a good match
	Action getActionForTokenWithInput(Token token, Action left, Action right, bool dynamic, bool throwSourceError);
	
	// addes all the matching actions in this and in all parents to out
	void getActions(string text, vector<Action>& out, bool dynamic);
	void getActions(Operator, vector<Action>& out);
	
	void getMatches(vector<AstNodeBase*>& out, string text, bool checkActions, bool checkDynamic, bool checkWhatev);
	
private:
	
	class IdMap
	{
	public:
		void add(string key, AstNode node);
		void get(string key, vector<AstNodeBase*>& out);
		
	private:
		unordered_map<string, vector<AstNode>> nodes;
	};
	
	// the only constructor, is private so use a make function instead
	NamespaceData(Namespace parentIn, shared_ptr<StackFrame> stackFrameIn, string nameIn="");
	
	// the name of this namespace, for debugging purposes only
	string myName;
	
	// this namespaces parent
	shared_ptr<NamespaceData> parent;
	
	// the stack frame this namespace uses
	// 	does not have a reference back because there can be many namespaces in one stack frame
	shared_ptr<StackFrame> stackFrame;
	
	// contains all actions that can be used anywhere, and don't rely on dynamic content like variables
	IdMap actions;
	
	// contains actions that use runtime data, like variables
	IdMap dynamicActions;
	
	// contains actions that use a whatev type
	IdMap whatevActions;
	
	// contains all types in this namespace
	IdMap types;
};

class AstActionWrapper: public AstNodeBase
{
public:
	
	static unique_ptr<AstActionWrapper> make(Action actionIn) {
		auto out = unique_ptr<AstActionWrapper>(new AstActionWrapper);
		out->inLeftType=actionIn->getInLeftType();
		out->inRightType=actionIn->getInRightType();
		out->returnType=actionIn->getReturnType();
		out->action=actionIn;
		out->dynamic=true; // shouldn't matter
		out->ns=nullptr; // shouldn't matter
		out->inputHasBeenSet=true;
		return out;
	}
	
	string getString() {return "action wrapper node";}
	
	AstNode makeCopy(bool copyCache)
	{
		auto out=new AstActionWrapper;
		copyToNode(out, true);
		return AstNode(out);
	}
	
	void resolveAction()
	{
		throw PineconeError("AstActionWrapper::resolveAction called, which it shouldn't have been", INTERNAL_ERROR);
	}
	
	Token getToken() {return nullptr;}
};

