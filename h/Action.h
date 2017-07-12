#pragma once

#include "StackFrame.h"
#include "Type.h"
#include "CppProgram.h"
#include <functional>
#include <memory>
#include <vector>
#include <string>
#include "utils/stringDrawing.h"


#include <stdlib.h> // malloc() and free() on some systems

using std::shared_ptr;
using std::unique_ptr;
using std::to_string;
using std::function;
using std::vector;

class ActionData;

extern shared_ptr<ActionData> voidAction;

class ActionData
{
public:
	
	ActionData(Type returnTypeIn, Type inLeftTypeIn, Type inRightTypeIn);
	
	virtual ~ActionData() {}
	
	void setDescription(string in) {description=in;}
	
	//void setText(string in) {text=in;}
	//string getText() {return text;}
	string toString();
	string getTypesString();
	virtual bool isFunction() {return false;}
	
	Type& getReturnType() {return returnType;}
	Type& getInLeftType() {return inLeftType;}
	Type& getInRightType() {return inRightType;}
	//void* execute(void* inLeft, void* inRight);
	
	virtual string getDescription() {return description;}
	virtual void* execute(void* inLeft, void* inRight)=0;
	//virtual string getCSource(string inLeft="", string inRight="")=0;
	virtual void addToProg(shared_ptr<ActionData> inLeft, shared_ptr<ActionData> inRight, CppProgram* prog)
	{
		prog->comment("action '"+getDescription()+"' to cpp code not yet implemented");
	}
	void addToProg(CppProgram* prog) {addToProg(voidAction, voidAction, prog);}
	// void addToProg(Action inLeft, Action inRight, CppProgram* prog)
	
	string nameHint="";
	
protected:
	
	//string text;
	
	Type returnType;
	Type inLeftType;
	Type inRightType;
	string description;
	//virtual DataElem * privateExecute(DataElem * inLeft, DataElem * inRight)=0;
};

typedef shared_ptr<ActionData> Action;

class AstNodeBase;

//Action lambdaAction(Type returnTypeIn, function<void*(void*,void*)> lambdaIn, Type inLeftTypeIn, Type inRightTypeIn, string textIn);
Action lambdaAction(Type inLeftTypeIn, Type inRightTypeIn, Type returnTypeIn, function<void*(void*,void*)> lambdaIn, function<void(Action inLeft, Action inRight, CppProgram* prog)> addCppToProg, string textIn);
Action createNewVoidAction();

Action branchAction(Action leftInputIn, Action actionIn, Action rightInputIn);

Action functionAction(Action actionIn, shared_ptr<StackFrame> stackFameIn);
Action functionAction(unique_ptr<AstNodeBase> nodeIn, Type returnTypeIn, shared_ptr<StackFrame> stackFameIn);

Action andAction(Action firstActionIn, Action secondActionIn);
Action orAction(Action firstActionIn, Action secondActionIn);

Action ifAction(Action conditionIn, Action ifActionIn);
Action ifElseAction(Action conditionIn, Action ifActionIn, Action elseAction);

Action listAction(const std::vector<Action>& actionsIn, const std::vector<Action>& destroyersIn);

Action loopAction(Action conditionIn, Action loopActionIn);
Action loopAction(Action conditionIn, Action endActionIn, Action loopActionIn);

Action makeTupleAction(const std::vector<Action>& sourceActionsIn);
//Action getElemFromTupleAction(Action source, string name);
Action getElemFromTupleAction(Type source, string name);
Action cppTupleCastAction(Action actionIn, Type returnType);

Action varGetAction(size_t in, Type typeIn, string idIn);
Action varSetAction(size_t in, Type typeIn, string idIn);
Action globalGetAction(size_t in, Type typeIn, string idIn);
Action globalSetAction(size_t in, Type typeIn, string idIn);

class NamespaceData;
Action constGetAction(const void* in, Type typeIn, string idIn, shared_ptr<NamespaceData> ns);

Action typeGetAction(Type typeIn);

