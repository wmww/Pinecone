#include "../h/Token.h"
#include "../h/Action.h"
#include "../h/ErrorHandler.h"

#include <vector>
using std::vector;

//	returns the index of the close peren that matches the given open peren index
//		tokens: the token array to use
//		i: the index of an open peren
//		returns: the index of the close peren that matches
int skipPeren(const vector<Token>& tokens, int i);

//	recursivly parse tokens and return action
//		tokens: the tokens to parse
//		left: left most token to parse (inclusinve)
//		right: right most token to parse (inclusive)
//		precedence: the index in precedence of the operator to look for (even means left to right, odd right to left)
//		returns: pointer to action for that section of tokens
ActionPtr parseTokens(const vector<Token>& tokens, int left, int right, int precedenceLevel);

//	recursivly parse tokens and return action, this one just splits up by white space
//		tokens: the tokens to parse
//		left: left most token to parse (inclusive)
//		right: right most token to parse (inclusive)
//		returns: pointer to action for that section of tokens
ActionPtr parseTokens(const vector<Token>& tokens, int left, int right);

int skipPeren(const vector<Token>& tokens, int i)
{
	if (tokens[i]->getOp()!=opOpenPeren)
	{
		error.log("skipPeren called with index that is not an open parenthesis", INTERNAL_ERROR, tokens[i]);
		return i;
	}
}

ActionPtr parseTokens(const vector<Token>& tokens)
{
	//return parseTokens(tokens, 0, tokens.size());
}

ActionPtr parseTokens(const vector<Token>& tokens, int left, int right, int precedenceLevel)
{
	
}

ActionPtr parseTokens(const vector<Token>& tokens, int left, int right)
{
	for (int i=0; i<tokens.size(); i++)
	{
		if (tokens[i]->getOp()==opOpenPeren)
			i=skipPeren(tokens, i);
	}
}

