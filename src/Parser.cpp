#include "../h/Token.h"
#include "../h/Action.h"
#include "../h/ErrorHandler.h"

#include <vector>
using std::vector;

//	recursivly parse tokens and return action
//		tokens: the tokens to parse
//		left: left most token to parse (inclusinve)
//		right: right most token to parse (inclusive)
//		lr: if to parse left to right, right to left parsing is used if false
//		returns: pointer to action for that section of tokens
ActionPtr parseTokens(const vector<Token>& tokens, int left, int right, bool lr);

ActionPtr parseTokens(const vector<Token>& tokens)
{
	return voidAction;
}
