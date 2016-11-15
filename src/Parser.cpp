#include "../h/Token.h"
#include "../h/Action.h"
#include "../h/ActionTable.h"
#include "../h/ErrorHandler.h"
#include "../h/StackFrame.h"
#include "../h/AllOperators.h"
#include "../h/msclStringFuncs.h"

#include <vector>
using std::vector;

#include <iostream>
using std::cout;
using std::endl;

using std::min;
using std::max;
using std::pair;

extern StackFrame stdLibStackFrame;
extern ActionTablePtr stdLibActionTable;

//	unless otherwise noted, these are what the perams for the following functions mean
//		tokens: the tokens to parse
//		table: the table to use
//		left: left most token to parse (inclusive)
//		right: right most token to parse (inclusive)
//		returns: (if type is ActionPtr) the action pointer for that section of the program

//	parses a function and returns the functionAction for it
ActionPtr parseFunction(const vector<Token>& tokens, int left, int right);

//	splits a stream of tokens into a ListAction and calls parseExpression on each expression
ActionPtr parseTokenList(const vector<Token>& tokens, ActionTablePtr table, int left, int right);

//	recursivly parses a single expression (no action lists)
ActionPtr parseExpression(const vector<Token>& tokens, ActionTablePtr table, int left, int right);

//	splits an expression on the given token (should always be an operator)
//		index: the index to split on
//		returns: pointer to BranchAction that is the root of the split
ActionPtr splitExpression(const vector<Token>& tokens, ActionTablePtr table, int left, int right, int index);

//	returns the action for a single token
//		token: the token to get the action for
//		leftIn: the left input
//		rightIn: the right input
ActionPtr parseSingleToken(Token token, const ActionTablePtr table, ActionPtr leftIn, ActionPtr rightIn);

//	returns the index of the closing brace that matches the given opening brace index, works with (), [], and {}
//		tokens: the token array to use
//		start: the index of an open peren
//		returns: the index of the close peren that matches
int skipBrace(const vector<Token>& tokens, int start);

void parseChain(const vector<Token>& tokens, ActionTablePtr table, int left, int right, vector<ActionPtr>& out);

ActionPtr parseOperator(const vector<Token>& tokens, ActionTablePtr table, int left, int right, int index);

ActionPtr parseLiteral(Token token);

//ActionPtr parseType(const vector<Token>& tokens, ActionTablePtr table, int left, int right);
//ActionPtr parseSingleTypeElement(const vector<Token>& tokens, ActionTablePtr table, int& i, int right, string& name, Type& type);
ActionPtr parseTypeToken(Token token, ActionTablePtr table);

ActionPtr parseIdentifier(Token token, ActionTablePtr table, ActionPtr leftIn, ActionPtr rightIn);




int skipBrace(const vector<Token>& tokens, int start)
{
	Operator open, close;
	int step;
	
	Operator op=tokens[start]->getOp();
	
	if (tokens[start]->getOp()==ops->openPeren)
	{
		open=ops->openPeren;
		close=ops->closePeren;
		step=1;
	}
	else if (tokens[start]->getOp()==ops->closePeren)
	{
		open=ops->closePeren;
		close=ops->openPeren;
		step=-1;
	}
	else if (tokens[start]->getOp()==ops->openSqBrac)
	{
		open=ops->openSqBrac;
		close=ops->closeSqBrac;
		step=1;
	}
	else if (tokens[start]->getOp()==ops->closeSqBrac)
	{
		open=ops->closeSqBrac;
		close=ops->openSqBrac;
		step=-1;
	}
	else if (tokens[start]->getOp()==ops->openCrBrac)
	{
		open=ops->openCrBrac;
		close=ops->closeCrBrac;
		step=1;
	}
	else if (tokens[start]->getOp()==ops->closeCrBrac)
	{
		open=ops->closeCrBrac;
		close=ops->openCrBrac;
		step=-1;
	}
	else
	{
		error.log(FUNC + " called with index that is not a valid brace", INTERNAL_ERROR, tokens[start]);
		return start;
	}
	
	int c=1;
	int i=start;
	
	while(true)
	{
		i+=step;
		
		if (i>=int(tokens.size()))
		{
			error.log("no matching brace", SOURCE_ERROR, tokens[start]);
			return start;
		}
		
		if (tokens[i]->getOp()==open)
		{
			c++;
		}
		else if (tokens[i]->getOp()==close)
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
	
	cout << endl << putStringInBox(table->toString(), false, "function table") << endl;
	
	ActionPtr out=functionAction(actions, Void, Void, table->getStackFrame()->getSize(), "main_function");
	
	return out;
}

ActionPtr parseExpression(const vector<Token>& tokens, ActionTablePtr table, int left, int right)
{
	if (left>right)
	{
		error.log(string() + __FUNCTION__ + " sent left higher then right", INTERNAL_ERROR, tokens[left]);
		return voidAction;
	}
	else if (left==right)
	{
		return parseSingleToken(tokens[left], table, voidAction, voidAction);
	}
	
	vector<bool> isMinLeft(right-left+1);
	vector<bool> isMinRight(right-left+1);
	
	int lowest;
	
	lowest=10000; //an number bigger then any precedence
	
	for (int i=left; i<=right; i++)
	{
		isMinLeft[i]=false;
		isMinRight[i]=false;
	}
	
	for (int i=left; i<=right; i++)
	{
		Operator op=tokens[i]->getOp();
		
		if (op)
		{
			if (op==ops->openPeren || op==ops->openSqBrac || op==ops->openCrBrac)
			{
				int j=i;
				i=skipBrace(tokens, i);
				
				if (j==left && i==right)
				{
					if (op==ops->openPeren)
					{
						if (left+1<right)
							return parseTokenList(tokens, table, left+1, right-1);
						else
							return voidAction; // a rare place where a voidAction may actually be intended by the programmer
					}
					else if (op==ops->openSqBrac)
					{
						if (left+1<right)
							return parseTokenList(tokens, table, left+1, right-1);
						else
							return voidAction; // a rare place where a voidAction may actually be intended by the programmer
					}
					else if (op==ops->openCrBrac)
					{
						if (left+2==right)
						{
							return parseTypeToken(tokens[left+1], table);
						}
						else if (left+1<right)
						{
							error.log("complex type parsing not yet implemented (required in "+stringFromTokens(tokens, left, right)+")", SOURCE_ERROR, tokens[left]);
							return voidAction;
						}
						else
							return voidAction; // a rare place where a voidAction may actually be intended by the programmer
					}
					else
					{
						return voidAction;
					}
				}
			}
			else
			{
				if (op->getLeftPrece()<lowest)
				{
					isMinLeft[i]=true;
				}
			}
			
			lowest=min(lowest, op->getRightPrece());
		}
	}
	
	lowest=10000; //an number bigger then any precedence
	
	for (int i=right; i>=left; i--)
	{
		Operator op=tokens[i]->getOp();
		
		if (op)
		{
			if (op==ops->closePeren || op==ops->closeSqBrac || op==ops->closeCrBrac)
			{
				i=skipBrace(tokens, i);
			}
			else
			{
				if (op->getRightPrece()<lowest)
				{
					isMinRight[i]=true;
				}
				
				lowest=min(lowest, op->getLeftPrece());
			}
		}
	}
	
	for (int i=left; i<=right; i++)
	{
		if (isMinLeft[i] && isMinRight[i])
		{
			return splitExpression(tokens, table, left, right, i);
		}
	}
	
	error.log("could not find where to split expression '" + stringFromTokens(tokens, left, right) + "'", SOURCE_ERROR, tokens[left]);
		
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
			if (tokens[i]->getOp()==ops->openPeren)
				i=skipBrace(tokens, i);
				
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

void parseChain(const vector<Token>& tokens, ActionTablePtr table, int left, int right, vector<ActionPtr>& out)
{
	if (tokens[left]->getOp()==ops->pipe)
	{
		error.log("improper use of pipe", SOURCE_ERROR, tokens[left]);
		return;
	}
	else if (tokens[right]->getOp()==ops->pipe)
	{
		error.log("improper use of pipe", SOURCE_ERROR, tokens[right]);
		return;
	}
	
	int i;
	
	for (i=left+1; i<=right && tokens[i]->getOp()!=ops->pipe; i++) {}
	
	out.push_back(parseExpression(tokens, table, left, i-1));
	
	if (i<right)
		parseChain(tokens, table, i+1, right, out);
}

ActionPtr parseOperator(const vector<Token>& tokens, ActionTablePtr table, int left, int right, int i)
{
	Operator op=tokens[i]->getOp();
	
	if (op==ops->colon)
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
	else if (op==ops->ifOp)
	{
		vector<ActionPtr> rightActions;
		
		parseChain(tokens, table, i+1, right, rightActions);
		
		auto leftAction=parseExpression(tokens, table, left, i-1);
		
		auto conditionAction=table->addConverter(leftAction, Bool);
		
		if (conditionAction==voidAction)
		{
			error.log("could not use "+leftAction->getDescription()+" as if condition", SOURCE_ERROR, tokens[i]);
			return voidAction;
		}
		else
		{
			if (rightActions.size()==1)
			{
				return ifAction(conditionAction, rightActions[0]);
			}
			else if (rightActions.size()==2)
			{
				return ifElseAction(conditionAction, rightActions[0], rightActions[1]);
			}
			else
			{
				error.log("can not make if with chain of length " + to_string(rightActions.size()), SOURCE_ERROR, tokens[i]);
				return voidAction;
			}
		}
	}
	else if (op==ops->loop)
	{
		vector<ActionPtr> leftActions;
		
		parseChain(tokens, table, left, i-1, leftActions);
		
		auto rightAction=parseExpression(tokens, table, i+1, right);
		
		if (leftActions.size()==1)
		{
			auto conditionAction=table->addConverter(leftActions[0], Bool);
			
			if (conditionAction==voidAction)
			{
				error.log("could not use "+leftActions[0]->getDescription()+" as condition in while loop", SOURCE_ERROR, tokens[i]);
				return voidAction;
			}
			else
			{
				return loopAction(conditionAction, voidAction, rightAction);
			}
		}
		else if (leftActions.size()==2)
		{
			auto conditionAction=table->addConverter(leftActions[0], Bool);
			auto afterAction=leftActions[1];
			
			if (conditionAction==voidAction)
			{
				error.log("could not use "+leftActions[0]->getDescription()+" as condition in while loop", SOURCE_ERROR, tokens[i]);
				return voidAction;
			}
			else
			{
				return loopAction(conditionAction, afterAction, rightAction);
			}
		}
		else
		{
			error.log("can not make loop with chain of length " + to_string(leftActions.size()), SOURCE_ERROR, tokens[i]);
			return voidAction;
		}
	}
	else
	{
		error.log(string() + __FUNCTION__ + " sent unknown operator (" + to_string(left) + ", " + to_string(right) + ") "+op->getText(), INTERNAL_ERROR, tokens[i]);
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

/*ActionPtr parseType(const vector<Token>& tokens, ActionTablePtr table, int left, int right)
{
	vector<string> names;
	vector<Type> types;
	
	for (int i=left; i<=right; i++)
	{
		
	}
}*/

/*void parseSingleTypeElement(const vector<Token>& tokens, ActionTablePtr table, int& i, int right, string& name, Type& type)
{
	int end=right;
	if (i+1<right && tokens[i+1]->getOp()==ops->colon)
	{
		if (tokens[i]->getType()==TokenData::IDENTIFIER)
		{
			if (i)
			
			=parseType(tokens, table, int i+2, int right)->getReturnType()->getTypes()[0];
			
			if (tokens[i+2]->get)
			{
				
			}
		}
		else
		{
			error.log("label in type definition was not an identifier", SOURCE_ERROR, tokens[i]);
			return typeGetAction(newMetatype(Void), "Void");
		}
	}
}*/

ActionPtr parseTypeToken(Token token, ActionTablePtr table)
{
	if (token->getType()==TokenData::IDENTIFIER)
	{
		Type type=table->getType(token->getText());
		
		if (type)
		{
			return typeGetAction(type, token->getText());
		}
		else
		{
			error.log("could not find type "+token->getDescription(), SOURCE_ERROR, token);
			return typeGetAction(newMetatype(Void), "Void");
		}
	}
	else
	{
		error.log(FUNC+"called with non identifier token", INTERNAL_ERROR, token);
		return typeGetAction(newMetatype(Void), "Void");
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
		if (rightIn->getReturnType()->isVoid())
		{
			error.log("could not resolve '"+token->getText()+"'", SOURCE_ERROR, token);
				return voidAction;
		}
		else
		{
			Type type=rightIn->getReturnType();
			
			if (type->getType()==TypeBase::METATYPE)
			{
				table->addType(Type(new TypeBase(type->getTypes()[0], token->getText())));
				return voidAction;
			}
			else if (type->isCreatable())
			{
				size_t offset=table->getStackFrame()->getSize();
				table->getStackFrame()->addMember(type);
				
				ActionPtr getAction=varGetAction(offset, type, token->getText());
				ActionPtr setAction=varSetAction(offset, type, token->getText());
				out = branchAction(voidAction, setAction, rightIn);
				table->addAction(getAction);
				table->addAction(setAction);
				return out;
			} 
			else
			{
				error.log(string() + "type "+type->getName()+" not creatable", SOURCE_ERROR, token);
				return voidAction;
			}			
		}
	}
	else
	{
		return out;
	}
}

	
	
