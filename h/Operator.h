#pragma once

class TokenData;

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
	static shared_ptr<OperatorData> create(string textIn, int leftPrecedenceIn, int rightPrecedenceIn, bool overloadableIn)
	{
		shared_ptr<OperatorData> ptr(new OperatorData(textIn, leftPrecedenceIn, rightPrecedenceIn, overloadableIn));
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
	
	OperatorData(string textIn, int leftPrecedenceIn, int rightPrecedenceIn, bool overloadableIn)
	{
		text=textIn;
		leftPrecedence=leftPrecedenceIn;
		rightPrecedence=rightPrecedenceIn;
		overloadable=overloadableIn;
	}
	
	// the text of the operator, generally 1 or 2 characters
	string text;
	
	// the precedence of left and right inputs, if this is 0 then the operator does not take an input on that side
	// if even then precedence level is parced left to right. if odd then right to left
	int leftPrecedence;
	int rightPrecedence;
	
	// if this operator can be overloaded
	bool overloadable;
};

typedef shared_ptr<OperatorData> Operator;

const extern Operator opPlus;
const extern Operator opMinus;
const extern Operator opMultiply;
const extern Operator opDivide;
const extern Operator opEqual;
const extern Operator opGreater;
const extern Operator opLess;
const extern Operator opColon;
const extern Operator opDot;
const extern Operator opComma;
const extern Operator opIf;
const extern Operator opLoop;
const extern Operator opOpenPeren;
const extern Operator opClosePeren;

void getOperators(string text, vector<Operator>& out);

