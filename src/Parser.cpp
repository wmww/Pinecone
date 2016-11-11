#include "../h/Token.h"
#include "../h/Action.h"
#include "../h/ErrorHandler.h"

#include <vector>
using std::vector;

using std::min;
using std::max;

//	recursivly parse tokens and return action
//		tokens: the tokens to parse
//		left: left most token to parse (inclusinve)
//		right: right most token to parse (inclusive)
//		precedence: the index in precedence of the operator to look for (even means left to right, odd right to left)
//		returns: pointer to action for that section of tokens
//ActionPtr parseTokens(const vector<Token>& tokens, int left, int right, int precedenceLevel);

//	splits a stream of tokens into a ListAction and calls parseExpression on each expression
//		tokens: the tokens to parse
//		left: left most token to parse (inclusive)
//		right: right most token to parse (inclusive)
//		returns: pointer to ListAction for that section of tokens
ActionPtr parseTokenList(const vector<Token>& tokens, int left, int right);

//	recursivly parses a single expression (no action lists)
//		tokens: the tokens to parse
//		left: left most token to parse (inclusive)
//		right: right most token to parse (inclusive)
//		returns: pointer to root action for that section of tokens
ActionPtr parseExpression(const vector<Token>& tokens, int left, int right);

//	splits an expression on the given token (should always be an operator)
//		tokens: the tokens to parse
//		left: left most token to parse (inclusive)
//		right: right most token to parse (inclusive)
//		index: the index to split on
//		returns: pointer to BranchAction that is the root of the split
ActionPtr splitExpression(const vector<Token>& tokens, int left, int right, int index);

//	returns the action for a single token
//		token: the token to get the action for
//		returns: the action for the token
ActionPtr parseSingleToken(const Token token);

//	returns the index of the close peren that matches the given open peren index
//		tokens: the token array to use
//		start: the index of an open peren
//		returns: the index of the close peren that matches
int skipPeren(const vector<Token>& tokens, int start);

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
	return parseTokenList(tokens, 0, tokens.size()-1);
}

/*ActionPtr parseTokens(const vector<Token>& tokens, int left, int right, int precedenceLevel)
{
	int precedence=OperatorData::precedenceLevels[precedenceLevel];
	int start, end, step;
	
	if (precedenceLevel%2)
	{
		start=right-1;
		end=left;
		step=-1;
	}
	else
	{
		start=left+1;
		end=right;
		step=1;
	}
	
	for (int i=start; i!=end; i+=step)
	{
		Operator op=tokens[i]->getOp();
		
		if (op)
		{
			if (op->getLeftPrecedence()<precedence)
			{
				error.log("precedence level too low for current recursion level", INTERNAL_ERROR, tokens[i]);
				return voidAction;
			}
			else if (op->getLeftPrecedence()==precedence)
			{
				
			}
		}
	}
}*/

ActionPtr parseExpression(const vector<Token>& tokens, int left, int right)
{
	if (left==right)
		return parseSingleToken(tokens[left]);
	
	//the first bit is 0 if that token is a min on the left and 1 if not, second bit is the same but for the right
	vector<unsigned char> notMin(right-left+1);
	
	int lowest;
	
	lowest=10000; //an number bigger then any precedence
	
	for (int i=left; i<=right; i++)
	{
		Operator op=tokens[i]->getOp();
		
		if (op)
		{
			notMin[i]=0x00;
			
			if (op->getLeftPrece()>=lowest)
			{
				notMin[i]|=0x01;
			}
			
			lowest=min(lowest, op->getRightPrece());
		}
	}
	
	lowest=10000; //an number bigger then any precedence
	
	for (int i=right; i>=right; i--)
	{
		Operator op=tokens[i]->getOp();
		
		if (op)
		{
			if (op->getRightPrece()>=lowest)
			{
				notMin[i]|=0x02;
			}
			
			lowest=min(lowest, op->getLeftPrece());
		}
	}
	
	for (int i=left; i<=right; i++)
	{
		if (!notMin[i])
			return splitExpression(tokens, left, right, i);
	}
	
	return voidAction;
}

ActionPtr parseTokenList(const vector<Token>& tokens, int left, int right)
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
					return parseExpression(tokens, left, right);
				}
				else
				{
					actions.push_back(parseExpression(tokens, left, right));
					break;
				}
			}
			else if // if the left cant absorb the right and the right cant absorbe the left
				(
					(
						!tokens[i]->getOp()
						||
						!tokens[i]->getOp()->getRightPrece()
					) && (
						!tokens[i+1]->getOp()
						||
						!tokens[i+1]->getOp()->getLeftPrece()
					)
				)
			{
				actions.push_back(parseExpression(tokens, left, right));
				break;
			}
			
			i++;
		}
		
		left=i+1;
	}
	
	return listAction(actions);
}

ActionPtr splitExpression(const vector<Token>& tokens, int left, int right, int index)
{
	return voidAction;
}

ActionPtr parseSingleToken(const Token token)
{
	return voidAction;
}
