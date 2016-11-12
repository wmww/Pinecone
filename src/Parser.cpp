#include "../h/Token.h"
#include "../h/Action.h"
#include "../h/ActionTable.h"
#include "../h/ErrorHandler.h"
#include "../h/StackFrame.h"

#include <vector>
using std::vector;

using std::min;
using std::max;
using std::pair;

extern StackFrame stdLibStackFrame;
extern ActionTablePtr stdLibActionTable;

//	recursivly parse tokens and return action
//		tokens: the tokens to parse
//		left: left most token to parse (inclusinve)
//		right: right most token to parse (inclusive)
//		precedence: the index in precedence of the operator to look for (even means left to right, odd right to left)
//		returns: pointer to action for that section of tokens
//ActionPtr parseTokens(const vector<Token>& tokens, int left, int right, int precedenceLevel);

ActionPtr parseFunction(const vector<Token>& tokens, int left, int right);

//	splits a stream of tokens into a ListAction and calls parseExpression on each expression
//		tokens: the tokens to parse
//		left: left most token to parse (inclusive)
//		right: right most token to parse (inclusive)
//		returns: pointer to ListAction for that section of tokens
ActionPtr parseTokenList(const vector<Token>& tokens, ActionTablePtr table, int left, int right);

//	recursivly parses a single expression (no action lists)
//		tokens: the tokens to parse
//		left: left most token to parse (inclusive)
//		right: right most token to parse (inclusive)
//		returns: pointer to root action for that section of tokens
ActionPtr parseExpression(const vector<Token>& tokens, ActionTablePtr table, int left, int right);

//	splits an expression on the given token (should always be an operator)
//		tokens: the tokens to parse
//		left: left most token to parse (inclusive)
//		right: right most token to parse (inclusive)
//		index: the index to split on
//		returns: pointer to BranchAction that is the root of the split
ActionPtr splitExpression(const vector<Token>& tokens, ActionTablePtr table, int left, int right, int index);

//	returns the action for a single token
//		token: the token to get the action for
//		returns: the action for the token
ActionPtr parseSingleToken(Token token, const ActionTablePtr table, ActionPtr leftIn, ActionPtr rightIn);

//	returns the index of the close peren that matches the given open peren index
//		tokens: the token array to use
//		start: the index of an open peren
//		returns: the index of the close peren that matches
int skipPeren(const vector<Token>& tokens, int start);

ActionPtr parseOperator(const vector<Token>& tokens, ActionTablePtr table, int left, int right, int index);

ActionPtr parseLiteral(Token token);

ActionPtr parseIdentifier(Token token, ActionTablePtr table, ActionPtr leftIn, ActionPtr rightIn);

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

ActionPtr parseFunction(const vector<Token>& tokens, int left, int right)
{
	StackFrame frame;
	
	ActionTablePtr table(new ActionTable(stdLibActionTable, &frame));
	
	ActionPtr actions=parseTokenList(tokens, table, left, right);
	
	ActionPtr out=functionAction(actions, Void, Void, table->getStackFrame()->getSize(), "main_function");
	
	return out;
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

ActionPtr parseExpression(const vector<Token>& tokens, ActionTablePtr table, int left, int right)
{
	if (left==right)
		return parseSingleToken(tokens[left], table, voidAction, voidAction);
	
	vector<bool> isMinLeft(right-left+1);
	vector<bool> isMinRight(right-left+1);
	
	int lowest;
	
	lowest=10000; //an number bigger then any precedence
	
	for (int i=left; i<=right; i++)
	{
		isMinLeft[i]=false;
		
		Operator op=tokens[i]->getOp();
		
		if (op)
		{
			if (op->getLeftPrece()<lowest)
			{
				isMinLeft[i]=true;
			}
			
			lowest=min(lowest, op->getRightPrece());
		}
	}
	
	lowest=10000; //an number bigger then any precedence
	
	for (int i=right; i>=left; i--)
	{
		isMinRight[i]=false;
		
		Operator op=tokens[i]->getOp();
		
		if (op)
		{
			if (op->getRightPrece()<lowest)
			{
				isMinRight[i]=true;
			}
			
			lowest=min(lowest, op->getLeftPrece());
		}
	}
	
	for (int i=left; i<=right; i++)
	{
		if (isMinLeft[i] && isMinRight[i])
		{
			return splitExpression(tokens, table, left, right, i);
		}
	}
	
	error.log("could not find where to split expression", SOURCE_ERROR, tokens[left]);
	//error.log("range: " + ([&]()->string{string out; for (int i=left; i<=right; i++) {out+=tokens[i]->getText()+" ";} return out;})(), JSYK, tokens[left]);
	//error.log("isMin: " + ([&]()->string{string out; for (auto i: isMin) {out+="\n"+to_string(i.first)+", "+to_string(i.second);} return out;})(), JSYK, tokens[left]);
	
	return voidAction;
}

ActionPtr parseTokenList(const vector<Token>& tokens, ActionTablePtr table, int left, int right)
{
	vector<ActionPtr> actions;
	
	while (left<=right)
	{
		int i=left;
		
		while(true)
		{
			if (tokens[i]->getOp()==opOpenPeren)
				i=skipPeren(tokens, i);
				
			if (i>=right) // at the end
			{
				if (actions.empty())
				{
					return parseExpression(tokens, table, left, right);
				}
				else
				{
					actions.push_back(parseExpression(tokens, table, left, right));
					break;
				}
			}
			else if // if the left can't absorb the right and the right cant absorbe the left
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
				actions.push_back(parseExpression(tokens, table, left, i));
				break;
			}
			
			i++;
		}
		
		left=i+1;
	}
	
	return listAction(actions);
}

ActionPtr splitExpression(const vector<Token>& tokens, ActionTablePtr table, int left, int right, int i)
{
	Operator op=tokens[i]->getOp();
	
	if (!op)
	{
		error.log(string() + __FUNCTION__ + " called with an index that is not an operator (token: " + tokens[i]->getText() + ")", INTERNAL_ERROR, tokens[i]);
		return voidAction;
	}
	
	if (op->getOverloadable())
	{
		auto leftAction=(left==i)?voidAction:parseExpression(tokens, table, left, i-1);
		auto rightAction=(right==i)?voidAction:parseExpression(tokens, table, i+1, right);
		
		return table->makeBranchAction(tokens[i], leftAction, rightAction);
	}
	else
	{
		return parseOperator(tokens, table, left, right, i);
	}
}

ActionPtr parseOperator(const vector<Token>& tokens, ActionTablePtr table, int left, int right, int i)
{
	Operator op=tokens[i]->getOp();
	
	if (op==opColon)
	{
		if (i==left+1)
		{
			auto rightAction=(right==i)?voidAction:parseExpression(tokens, table, i+1, right);
			
			return parseSingleToken(tokens[left], table, voidAction, rightAction);
		}
		else
		{
			error.log("right now, ':' must have an identifier to its left, instead it had " + to_string(left) + "-" + to_string(i-1) + " which is of token type of " + TokenData::typeToString(tokens[left]->getType()), SOURCE_ERROR, tokens[i]);
			return voidAction;
		}
	}
	else if (op==opIf)
	{
		auto leftAction=parseExpression(tokens, table, left, i-1);
		auto rightAction=parseExpression(tokens, table, i+1, right);
		
		auto conditionAction=table->addConverter(leftAction, Bool);
		
		if (conditionAction==voidAction)
		{
			error.log("could not use "+leftAction->getDescription()+" as if condition", SOURCE_ERROR, tokens[i]);
			return voidAction;
		}
		else
		{
			return ifAction(conditionAction, rightAction);
		}
	}
	else if (op==opLoop)
	{
		auto leftAction=parseExpression(tokens, table, left, i-1);
		auto rightAction=parseExpression(tokens, table, i+1, right);
		
		auto conditionAction=table->addConverter(leftAction, Bool);
		
		if (conditionAction==voidAction)
		{
			error.log("could not use "+leftAction->getDescription()+" as condition in while loop", SOURCE_ERROR, tokens[i]);
			return voidAction;
		}
		else
		{
			return loopAction(conditionAction, rightAction);
		}
	}
	else
	{
		error.log("unknown operator "+op->getText(), INTERNAL_ERROR, tokens[i]);
		return voidAction;
	}
}

ActionPtr parseSingleToken(Token token, ActionTablePtr table, ActionPtr leftIn, ActionPtr rightIn)
{
	switch (token->getType())
	{
		
	case TokenData::LITERAL:
		return parseLiteral(token);
		break;
		
	case TokenData::IDENTIFIER:
		return parseIdentifier(token, table, leftIn, rightIn);
		break;
		
	default:
		error.log(string() + __FUNCTION__ + " called with token of invalid token type " + token->getDescription(), INTERNAL_ERROR, token);
		break;
	}
	
	return voidAction;
}

ActionPtr parseLiteral(Token token)
{
	if (token->getType()!=TokenData::LITERAL)
	{
		error.log(string() + __FUNCTION__ + " called with incorrect token type " + token->getDescription(), INTERNAL_ERROR, token);
		return voidAction;
	}
	
	string in=token->getText();
	
	if (in.empty())
		return nullptr;
	
	//bool floatingPoint=false;
	
	Type type=UnknownType;
	
	if (in.empty())
	{
		error.log("tried to make literal with empty string", INTERNAL_ERROR, token);
	}
	
	if ((in[0]>='0' && in[0]<='9') || in[0]=='.')
	{
		type=Int;
		
		for (auto i=in.begin(); i!=in.end(); ++i)
		{
			if (*i=='.' || *i=='_')
			{
				type=Dub;
				break;
			}
		}
		
		if (in.back()=='d' || in.back()=='f')
		{
			type=Dub;
			in.pop_back();
		}
		else if (in.back()=='i')
		{
			type=Int;
			in.pop_back();
		}
		else if (in.back()=='b')
		{
			type=Bool;
			in.pop_back();
		}
	}
	
	if (type==Int || type==Int)
	{
		int val=0;
		
		for (auto i=in.begin(); i!=in.end(); ++i)
		{
			if (*i<'0' || *i>'9')
			{
				error.log(string() + "bad character '" + *i + "' found in number '" + in + "'", SOURCE_ERROR, token);
				return nullptr;
			}
			
			val=val*10+(*i-'0');
		}
		
		if (type==Bool)
		{
			bool out=(val!=0);
			return constGetAction(&out, type, token->getText());
		}
		else
		{
			int out=val;
			return constGetAction(&out, type, token->getText());
		}
	}
	else if (type==Dub) //floating point
	{
		double val=0;
		int pointPos=0;
		
		for (auto i=in.begin(); i!=in.end(); ++i)
		{
			if (*i=='.' || *i=='_')
			{
				if (pointPos==0)
				{
					pointPos=10;
				}
				else
				{
					error.log(string() + "multiple decimal points found in number '" + in + "'", SOURCE_ERROR, token);
					return voidAction;
				}
			}
			else if (*i>='0' && *i<='9')
			{
				if (pointPos)
				{
					val+=(double)(*i-'0')/pointPos;
					pointPos*=10;
				}
				else
				{
					val=val*10+(*i-'0');
				}
			}
			else
			{
				error.log(string() + "bad character '" + *i + "' found in number '" + in + "'", SOURCE_ERROR, token);
				return voidAction;
			}
		}
		
		double out=val;
		return constGetAction(&out, type, token->getText());
	}
	else
	{
		error.log("tried to make literal with invalid type of " + type->toString(), INTERNAL_ERROR, token);
		return voidAction;
	}
}

ActionPtr parseIdentifier(Token token, ActionTablePtr table, ActionPtr leftIn, ActionPtr rightIn)
{
	if (token->getType()!=TokenData::IDENTIFIER)
	{
		error.log(string() + __FUNCTION__ + " called with incorrect token type " + token->getDescription(), INTERNAL_ERROR, token);
		return voidAction;
	}
	
	ActionPtr out=table->makeBranchAction(token, leftIn, rightIn);
	
	if (out==voidAction)
	{
		Type type=rightIn->getReturnType();
		
		if (type->isCreatable())
		{
			size_t offset=table->getStackFrame()->getSize();
			table->getStackFrame()->addMember(type);
			
			ActionPtr getAction=varGetAction(offset, type, token->getText());
			ActionPtr setAction=varSetAction(offset, type, token->getText());
			out = branchAction(voidAction, setAction, rightIn);
			table->addAction(getAction);
			table->addAction(setAction);
		}
		else
		{
			error.log(string() + "type "+type->getName()+" not creatable", SOURCE_ERROR, token);
			out=voidAction;
		}
	}
	
	return out;
}

	
	
