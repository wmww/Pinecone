#include "../h/Token.h"
#include "../h/Action.h"
#include "../h/Namespace.h"
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
extern Namespace stdLibNamespace;

//	unless otherwise noted, these are what the perams for the following functions mean
//		tokens: the tokens to parse
//		table: the table to use
//		left: left most token to parse (inclusive)
//		right: right most token to parse (inclusive)
//		returns: (if type is Action) the action pointer for that section of the program

//	parses a function and returns the functionAction for it
Action parseFunction(const vector<Token>& tokens, int left, int right, Type leftInType, Type rightInType);

//	splits a stream of tokens into a ListAction and calls parseExpression on each expression
Action parseTokenList(const vector<Token>& tokens, Namespace table, int left, int right);

//	recursivly parses a single expression (no action lists)
Action parseExpression(const vector<Token>& tokens, Namespace table, int left, int right);

//	splits an expression on the given token (should always be an operator)
//		index: the index to split on
//		returns: pointer to BranchAction that is the root of the split
Action splitExpression(const vector<Token>& tokens, Namespace table, int left, int right, int index);

//	returns the action for a single token
//		token: the token to get the action for
//		leftIn: the left input
//		rightIn: the right input
Action parseSingleToken(Token token, const Namespace table, Action leftIn, Action rightIn);

//	returns the index of the closing brace that matches the given opening brace index, works with (), [], and {}
//		tokens: the token array to use
//		start: the index of an open peren
//		returns: the index of the close peren that matches
int skipBrace(const vector<Token>& tokens, int start);

void parseChain(const vector<Token>& tokens, Namespace table, int left, int right, vector<Action>& out);

Action parseOperator(const vector<Token>& tokens, Namespace table, int left, int right, int index);

Action parseLiteral(Token token);

Action parseType(const vector<Token>& tokens, Namespace table, int left, int right);
//Action parseSingleTypeElement(const vector<Token>& tokens, Namespace table, int& i, int right, string& name, Type& type);
Type parseTypeToken(Token token, Namespace table);

Action parseIdentifier(Token token, Namespace table, Action leftIn, Action rightIn);

void parseIdentifierConst(Token token, Namespace table, Action rightIn);



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

Action parseFunction(const vector<Token>& tokens, int left, int right, Type leftInType, Type rightInType)
{
	StackFrame frame;
	
	Namespace nmspace=stdLibNamespace->makeChildAndFrame();
	
	Action actions=parseTokenList(tokens, nmspace, left, right);
	
	cout << endl << putStringInBox(nmspace->getString(), false, "function table") << endl;
	
	Action out=functionAction(actions, leftInType, rightInType, nmspace->getStackFrame());
	
	return out;
}

Action parseExpression(const vector<Token>& tokens, Namespace table, int left, int right)
{
	error.log("parsing expression: "+stringFromTokens(tokens, left, right), JSYK);
	
	if (left>right)
	{
		//error.log(string() + __FUNCTION__ + " sent left higher then right", INTERNAL_ERROR, tokens[left]);
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
							return typeGetAction(parseTypeToken(tokens[left+1], table));
						}
						else if (left+1<right-1)
						{
							return parseType(tokens, table, left+1, right-1);
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

Action parseTokenList(const vector<Token>& tokens, Namespace table, int left, int right)
{
	vector<Action> actions;
	
	while (left<=right)
	{
		int i=left;
		
		while(true)
		{
			auto op=tokens[i]->getOp();
			
			if (op==ops->openPeren || op==ops->openSqBrac || op==ops->openCrBrac)
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

Action splitExpression(const vector<Token>& tokens, Namespace table, int left, int right, int i)
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
		
		return table->getActionForTokenWithInput(tokens[i], leftAction, rightAction);
	}
	else
	{
		return parseOperator(tokens, table, left, right, i);
	}
}

void parseChain(const vector<Token>& tokens, Namespace table, int left, int right, vector<Action>& out)
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

Action parseOperator(const vector<Token>& tokens, Namespace table, int left, int right, int i)
{
	Operator op=tokens[i]->getOp();
	
	if (op==ops->colon)
	{
		if (i==left+1 && tokens[left]->getType()==TokenData::IDENTIFIER)
		{
			auto rightAction=parseExpression(tokens, table, i+1, right);
			
			return parseSingleToken(tokens[left], table, voidAction, rightAction);
		}
		else
		{
			auto action=parseExpression(tokens, table, left, i-1);
			
			auto type=action->getReturnType();
			
			if (type->getType()==TypeBase::METATYPE)
			{
				auto func=parseFunction(tokens, i+1, right, Void, type);
				
				return func;
			}
			else
			{
				error.log("right now, ':' must have an either an identifier or a type to its left, instead it had '"+stringFromTokens(tokens, left, i-1)+"'", SOURCE_ERROR, tokens[i]);
				return voidAction;
			}
		}
	}
	else if (op==ops->doubleColon)
	{
		if (i==left+1 && tokens[left]->getType()==TokenData::IDENTIFIER)
		{
			Action rigntAction=parseExpression(tokens, table, i+1, right);
			
			parseIdentifierConst(tokens[left], table, rigntAction);
			
			return voidAction;
		}
		else
		{
			error.log("'::' must have an identifier to its left", SOURCE_ERROR, tokens[i]);
			return voidAction;
		}
	}
	else if (op==ops->ifOp)
	{
		vector<Action> rightActions;
		
		parseChain(tokens, table, i+1, right, rightActions);
		
		auto leftAction=parseExpression(tokens, table, left, i-1);
		
		auto conditionAction=table->getActionConvertedToType(leftAction, Bool);
		
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
		vector<Action> leftActions;
		
		parseChain(tokens, table, left, i-1, leftActions);
		
		auto rightAction=parseExpression(tokens, table, i+1, right);
		
		if (leftActions.size()==1)
		{
			auto conditionAction=table->getActionConvertedToType(leftActions[0], Bool);
			
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
			auto conditionAction=table->getActionConvertedToType(leftActions[0], Bool);
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
	else if (op==ops->comma)
	{
		vector<Action> out;
		
		for (int j=left; j<=right+1; j++)
		{
			if (tokens[j]->getOp()==ops->comma || j==right+1)
			{
				if (j==left)
				{
					error.log("invalid use of ','", SOURCE_ERROR, tokens[j]);
					return voidAction;
				}
				
				out.push_back(parseExpression(tokens, table, left, j-1));
				left=j+1;
			}
		}
		
		return makeTupleAction(out);
	}
	/*else if (op==ops->dot)
	{
		if (left+1==i && i+1==right && tokens[left]->getType()==TokenData::LITERAL && tokens[right]->getType()==TokenData::LITERAL)
	}*/
	else
	{
		error.log(string() + FUNC + " sent unknown operator (" + to_string(left) + ", " + to_string(right) + ") "+op->getText(), INTERNAL_ERROR, tokens[i]);
		return voidAction;
	}
}

Action parseSingleToken(Token token, Namespace table, Action leftIn, Action rightIn)
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

Action parseLiteral(Token token)
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
	
	Type type=Unknown;
	
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
		error.log("tried to make literal with invalid type of " + type->getString(), INTERNAL_ERROR, token);
		return voidAction;
	}
}

/*void parseSingleTypeElement(const vector<Token>& tokens, Namespace table, int& i, int right, string& name, Type& type)
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

Action parseType(const vector<Token>& tokens, Namespace table, int left, int right)
{
	TupleTypeMaker tuple;
	
	while (left<=right)
	{
		if (left+1<right && tokens[left+1]->getOp()==ops->colon)
		{
			if (tokens[left]->getType()!=TokenData::IDENTIFIER)
			{
				error.log("identifier must be to the left of ':' in type", SOURCE_ERROR, tokens[left]);
				return voidAction;
			}
			
			if (tokens[left+2]->getType()!=TokenData::IDENTIFIER)
			{
				error.log("identifier must be to the right of ':' in type", SOURCE_ERROR, tokens[left]);
				return voidAction;
			}
			
			tuple.add(tokens[left]->getText(), parseTypeToken(tokens[left+2], table));
			
			left+=3;
		}
		else
		{
			tuple.add(parseTypeToken(tokens[left+1], table));
			left+=1;
		}
	}
	
	return typeGetAction(tuple.get());
}

Type parseTypeToken(Token token, Namespace table)
{
	if (token->getType()==TokenData::IDENTIFIER)
	{
		Type type=table->getType(token->getText());
		
		if (type)
		{
			return type;
		}
		else
		{
			error.log("could not find type "+token->getDescription(), SOURCE_ERROR, token);
			return Void;
		}
	}
	else
	{
		error.log(FUNC+"called with non identifier token", INTERNAL_ERROR, token);
		return Void;
	}
}

Action parseIdentifier(Token token, Namespace table, Action leftIn, Action rightIn)
{
	if (token->getType()!=TokenData::IDENTIFIER)
	{
		error.log(string() + __FUNCTION__ + " called with incorrect token type " + token->getDescription(), INTERNAL_ERROR, token);
		return voidAction;
	}
	
	Action out=table->getActionForTokenWithInput(token, leftIn, rightIn);
	
	if (out==voidAction)
	{
		Type type=rightIn->getReturnType();
		
		if (type->isVoid()) //probably dev was trying to use an identifier that doesn't exist
		{
			error.log("could not resolve '"+token->getText()+"'", SOURCE_ERROR, token);
				return voidAction;
		}
		else if (type->getType()==TypeBase::METATYPE)
		{
			//table->addType(Type(new TypeBase(type->getTypes()[0], token->getText())), token->getText());
			
			error.log("metatype handeling in "+FUNC+" not yet implemented", INTERNAL_ERROR);
			
			return voidAction;
		}
		else if (type->isCreatable())
		{
			size_t offset=table->getStackFrame()->getSize();
			table->getStackFrame()->addMember(type);
			
			Action getAction=varGetAction(offset, type, token->getText());
			Action setAction=varSetAction(offset, type, token->getText());
			out = branchAction(voidAction, setAction, rightIn);
			table->addAction(getAction, token->getText());
			table->addAction(setAction, token->getText());
			return out;
		}
		else
		{
			error.log(string() + "type "+type->getString()+" not creatable", SOURCE_ERROR, token);
			return voidAction;
		}
	}
	else
	{
		return out;
	}
}

void parseIdentifierConst(Token token, Namespace table, Action rightIn)
{
	table->addAction(rightIn, token->getText());
}


