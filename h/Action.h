#pragma once

#include "StackFrame.h"
#include "Type.h"
#include <functional>
#include <memory>
#include <vector>

using std::shared_ptr;
using std::unique_ptr;
using std::to_string;
using std::function;
using std::vector;

class ActionData
{
public:
	
	ActionData(Type returnTypeIn, Type inLeftTypeIn, Type inRightTypeIn);
	
	virtual ~ActionData() {}
	
	void setDescription(string in) {description=in;}
	
	//void setText(string in) {text=in;}
	//string getText() {return text;}
	string toString();
	
	Type& getReturnType() {return returnType;}
	Type& getInLeftType() {return inLeftType;}
	Type& getInRightType() {return inRightType;}
	//void* execute(void* inLeft, void* inRight);
	
	virtual string getDescription() {return description;}
	virtual void* execute(void* inLeft, void* inRight)=0;
	virtual string getCSource(string inLeft="", string inRight="")=0;
		
protected:
	
	//string text;
	
	Type returnType;
	Type inLeftType;
	Type inRightType;
	string description;
	//virtual DataElem * privateExecute(DataElem * inLeft, DataElem * inRight)=0;
};

typedef shared_ptr<ActionData> Action;

extern Action voidAction;

Action lambdaAction(Type returnTypeIn, function<void*(void*,void*)> lambdaIn, Type inLeftTypeIn, Type inRightTypeIn, string textIn);
Action createNewVoidAction();

Action branchAction(Action leftInputIn, Action actionIn, Action rightInputIn);

Action functionAction(Action actionIn, shared_ptr<StackFrame> stackFameIn);

Action ifAction(Action conditionIn, Action ifActionIn);
Action ifElseAction(Action conditionIn, Action ifActionIn, Action elseAction);

Action listAction(const std::vector<Action>& actionsIn);

Action loopAction(Action conditionIn, Action loopActionIn);
Action loopAction(Action conditionIn, Action endActionIn, Action loopActionIn);

Action makeTupleAction(const std::vector<Action>& sourceActionsIn);

Action varGetAction(size_t in, Type typeIn, string idIn);
Action varSetAction(size_t in, Type typeIn, string idIn);
Action constGetAction(void* in, Type typeIn, string idIn);

Action typeGetAction(Type typeIn);

