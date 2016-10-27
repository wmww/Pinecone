#pragma once

#include "ElementData.h"

#include <memory>
using std::shared_ptr;

#include <string>
using std::string;

#include <vector>
using std::vector;

#include <iostream>

class OperatorBase
{
public:
	
	OperatorBase(string textIn, bool leftIn, bool rightIn, bool overridableIn)
	{
		text=textIn;
		takesLeftInput=leftIn;
		takesRightInput=rightIn;
		overridable=overridableIn;
		operators.push_back(shared_ptr<OperatorBase>(this));
	}
	
	~OperatorBase()
	{
		std::cout << text << std::endl;
	}
	
	string getText() {return text;}
	bool getTakesLeftInput() {return takesLeftInput;}
	bool getTakesRightInput() {return takesRightInput;}
	bool getOverridable() {return overridable;}
	
	static vector<shared_ptr<OperatorBase>> operators;
	
private:
	
	string text;
	bool takesLeftInput;
	bool takesRightInput;
	bool overridable;
};

typedef shared_ptr<OperatorBase> Operator;

extern Operator opPlus;
extern Operator opMinus;
extern Operator opColon;
extern Operator opDot;
extern Operator opOpenPeren;
extern Operator opClosePeren;

void getOperators(vector<Operator>& out, string text, ElementData data);

