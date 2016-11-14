#pragma once

class TokenData;

#include <memory>
using std::shared_ptr;

#include <string>
using std::string;

class AllOperators;

// describes an operator
// NOTE: there will only be one instance of OperatorData for +, not one for every overload of +
class OperatorData
{
public:
	
	string getText() {return text;}
	int getLeftPrece() {return leftPrecedence;}
	int getRightPrece() {return rightPrecedence;}
	bool getOverloadable() {return overloadable;}
	
private:
	
	friend AllOperators;
	
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

