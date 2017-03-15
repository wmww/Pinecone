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
	
	enum InputTaken
	{
		LEFT,
		RIGHT,
		BOTH,
	};
	
	string getText() {return text;}
	int getPrecedence() {return precedence;}
	bool isOverloadable() {return overloadable;}
	bool takesLeftInput() {return input==BOTH || input==LEFT;}
	bool takesRightInput() {return input==BOTH || input==RIGHT;}
	
private:
	
	friend AllOperators;
	
	OperatorData(string textIn, int precedenceIn, InputTaken inputIn, bool overloadableIn)
	{
		text=textIn;
		precedence=precedenceIn;
		input=inputIn;
		overloadable=overloadableIn;
	}
	
	// the text of the operator, generally 1 or 2 characters
	string text;
	
	// the precedence of left and right inputs, if this is 0 then the operator does not take an input on that side
	// if even then precedence level is parced left to right. if odd then right to left
	int precedence;
	
	// if this operator can be overloaded
	bool overloadable;
	
	// if this operator takes input from the left, right or both
	InputTaken input;
};

typedef shared_ptr<OperatorData> Operator;

