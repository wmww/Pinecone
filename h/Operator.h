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

// describes an operator
// NOTE: there will only be one instance of OperatorData for +, not one for every overload of +
class OperatorData
{
public:
	
	// this is the only way to make an operator, and should only be called when setting up all the global operators at the top of Operator.cpp
	static shared_ptr<OperatorData> create(string textIn, int leftPrecedenceIn, int rightPrecedenceIn, bool leftParseLRIn, bool rightParseLRIn, bool overloadableIn)
	{
		shared_ptr<OperatorData> ptr(new OperatorData(textIn, leftPrecedenceIn, rightPrecedenceIn, leftParseLRIn, rightParseLRIn, overloadableIn));
		operators[textIn]=ptr;
		
		for (auto i=precedenceLevels.begin();; i++)
		{
			if (i==precedenceLevels.end() || *i>leftPrecedenceIn)
			{
				precedenceLevels.insert(i, leftPrecedenceIn);
				break;
			}
			else if (*i==leftPrecedenceIn)
				break;
		}
		
		for (auto i=precedenceLevels.begin();; i++)
		{
			if (i==precedenceLevels.end() || *i>rightPrecedenceIn)
			{
				precedenceLevels.insert(i, rightPrecedenceIn);
				break;
			}
			else if (*i==rightPrecedenceIn)
				break;
		}
		
		//operators.push_back(ptr);
		return ptr;
	}
	
	string getText() {return text;}
	int getLeftPrecedence() {return leftPrecedence;}
	int getRightPrecedence() {return rightPrecedence;}
	bool getOverloadable() {return overloadable;}
	
	//static vector<shared_ptr<OperatorData>> operators;
	
	static unordered_map<string, shared_ptr<OperatorData>> operators;
	
	// stores the precedence levels that are used in order
	static vector<int> precedenceLevels;
	
private:
	
	OperatorData(string textIn, int leftPrecedenceIn, int rightPrecedenceIn, bool leftParseLRIn, bool rightParseLRIn, bool overloadableIn)
	{
		text=textIn;
		leftPrecedence=leftPrecedenceIn;
		rightPrecedence=rightPrecedenceIn;
		leftParseLR=leftParseLRIn;
		rightParseLR=rightParseLRIn;
		overloadable=overloadableIn;
	}
	
	// the text of the operator, generally 1 or 2 characters
	string text;
	
	// the precedence of left and right inputs, if this is -1 then the operator does not take an input on that side
	int leftPrecedence;
	int rightPrecedence;
	
	// if to parse the left or right input left to right (right to left if false)
	bool leftParseLR;
	bool rightParseLR;
	
	// if this operator can be overloaded
	bool overloadable;
};

typedef shared_ptr<OperatorData> Operator;

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

