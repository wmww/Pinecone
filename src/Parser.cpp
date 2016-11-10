#include "../h/Token.h"
#include "../h/Action.h"
#include "../h/ErrorHandler.h"

#include <vector>
using std::vector;

//	returns the index of the close peren that matches the given open peren index
//		tokens: the token array to use
//		start: the index of an open peren
//		returns: the index of the close peren that matches
int skipPeren(const vector<Token>& tokens, int start);

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

int skipPeren(const vector<Token>& tokens, int start)
{
	if (tokens[start]->getOp()!=opOpenPeren)
	{
		error.log("skipPeren called with index that is not an open parenthesis", INTERNAL_ERROR, tokens[start]);
		return start;
	}
	
	int c=1;
	int i=start;
	
	while(true)
	{
		i++;
		
		if (i>=int(tokens.size()))
		{
			error.log("no closing parenthesis", SOURCE_ERROR, tokens[start]);
			return start;
		}
		
		if (tokens[i]->getOp()==opOpenPeren)
		{
			c++;
		}
		else if (tokens[i]->getOp()==opClosePeren)
		{
			c--;
			
			if (c<=0)
			{
				return i;
			}
		}
	}
}

ActionPtr parseTokens(const vector<Token>& tokens)
{
	return parseTokens(tokens, 0, tokens.size()-1);
}

ActionPtr parseTokens(const vector<Token>& tokens, int left, int right, int precedenceLevel)
{
	return voidAction;
}

ActionPtr parseTokens(const vector<Token>& tokens, int left, int right)
{
	vector<ActionPtr> actions;
	
	while (left<=right)
	{
		int i=left;
		
		while(true)
		{
			if (tokens[i]->getOp()==opOpenPeren)
				i=skipPeren(tokens, i);
				
			if (i>=right)
			{
				if (actions.empty())
				{
					return parseTokens(tokens, left, right, OperatorData::precedenceLevels.back());
				}
				else
				{
					actions.push_back(parseTokens(tokens, left, right, 0));
					break;
				}
			}
			else if // if the left cant absorb the right and the right cant absorbe the left
				(
					(
						!tokens[i]->getOp()
						||
						!tokens[i]->getOp()->getRightPrecedence()
					) && (
						!tokens[i+1]->getOp()
						||
						!tokens[i+1]->getOp()->getLeftPrecedence()
					)
				)
			{
				actions.push_back(parseTokens(tokens, left, right, OperatorData::precedenceLevels.back()));
				break;
			}
			
			i++;
		}
		
		left=i+1;
	}
	
	return listAction(actions);
}

