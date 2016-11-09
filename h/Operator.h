#pragma once

class Token;

#include <memory>
using std::shared_ptr;

#include <string>
using std::string;

#include <vector>
using std::vector;

#include <unordered_map>
using std::unordered_map;

#include <iostream>

class OperatorBase
{
public:
	
	static shared_ptr<OperatorBase> create(string textIn, bool leftIn, bool rightIn, bool overridableIn)
	{
		shared_ptr<OperatorBase> ptr(new OperatorBase(textIn, leftIn, rightIn, overridableIn));
		operators[textIn]=ptr;
		//operators.push_back(ptr);
		return ptr;
	}
	
	string getText() {return text;}
	bool getTakesLeftInput() {return takesLeftInput;}
	bool getTakesRightInput() {return takesRightInput;}
	bool getOverridable() {return overridable;}
	
	//static vector<shared_ptr<OperatorBase>> operators;
	
	static unordered_map<string, shared_ptr<OperatorBase>> operators;
	
private:
	
	OperatorBase(string textIn, bool leftIn, bool rightIn, bool overridableIn)
	{
		text=textIn;
		takesLeftInput=leftIn;
		takesRightInput=rightIn;
		overridable=overridableIn;
	}
	
	string text;
	bool takesLeftInput;
	bool takesRightInput;
	bool overridable;
};

typedef shared_ptr<OperatorBase> Operator;

extern Operator opPlus;
extern Operator opMinus;
extern Operator opMultiply;
extern Operator opDivide;
extern Operator opEqual;
extern Operator opGreater;
extern Operator opLess;
extern Operator opColon;
extern Operator opDot;
extern Operator opComma;
extern Operator opIf;
extern Operator opLoop;
extern Operator opOpenPeren;
extern Operator opClosePeren;

void getOperators(string text, vector<Operator>& out);

