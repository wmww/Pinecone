#include "../h/Token.h"
#include "../h/Action.h"
#include "../h/ErrorHandler.h"

#include <vector>
using std::vector;



//	recursivly parse tokens and return action
//		tokens: the tokens to parse
//		left: left most token to parse (inclusinve)
//		right: right most token to parse (inclusive)
//		precedence: the precedence of the operator to look for (even means left to right, odd right to left)
//		returns: pointer to action for that section of tokens
ActionPtr parseTokens(const vector<Token>& tokens, int left, int right, int precedence);

ActionPtr parseTokens(vector<Token>& tokens)
{
	return parseTokens(tokens, 0, tokens.size(), OperatorData::precedenceLevels.back());
}

ActionPtr parseTokens(const vector<Token>& tokens, int left, int right, int precedence)
{
	
}

