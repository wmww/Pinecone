#include "../h/Token.h"
#include "../h/AstNode.h"
#include "../h/Namespace.h"
#include "../h/ErrorHandler.h"
#include "../h/StackFrame.h"
#include "../h/AllOperators.h"
#include "../h/msclStringFuncs.h"
#include "../h/AstNode.h"

#include <vector>
using std::vector;

#include <iostream>
using std::cout;
using std::endl;

using std::min;
using std::max;
using std::pair;

//	unless otherwise noted, these are what the perams for the following functions mean
//		tokens: the tokens to parse
//		table: the table to use
//		left: left most token to parse (inclusive)
//		right: right most token to parse (inclusive)
//		returns: (if type is AstNode) the action pointer for that section of the program

//	splits a stream of tokens into a ListAstNode and calls parseExpression on each expression
AstNode parseTokenList(const vector<Token>& tokens, int left, int right);

//	recursivly parses a single expression (no action lists)
AstNode parseExpression(const vector<Token>& tokens, int left, int right);

//	returns the index of the closing brace that matches the given opening brace index, works with (), [], and {}
//		tokens: the token array to use
//		start: the index of an open peren
//		returns: the index of the close peren that matches
int skipBrace(const vector<Token>& tokens, int start);

void parseChain(const vector<Token>& tokens, int left, int right, vector<AstNode>& out);

AstNode parseOperator(const vector<Token>& tokens, int left, int right, int index);

//AstNode parseLiteral(Token token);

unique_ptr<AstType> parseType(const vector<Token>& tokens, int left, int right);
//AstNode parseSingleTypeElement(const vector<Token>& tokens, int& i, int right, string& name, Type& type);
//Type parseTypeToken(Token token);

//AstNode parseIdentifier(Token token, AstNode leftIn, AstNode rightIn);

//void parseIdentifierConst(Token token, AstNode rightIn);

AstNode astNodeFromTokens(const vector<Token>& tokens)
{
	return parseTokenList(tokens, 0, tokens.size()-1);
}

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
		throw PineconeError(FUNC + " called with index that is not a valid brace", INTERNAL_ERROR, tokens[start]);
	}
	
	int c=1;
	int i=start;
	
	while(true)
	{
		i+=step;
		
		if (i>=int(tokens.size()))
		{
			throw PineconeError("no matching brace", SOURCE_ERROR, tokens[start]);
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

/*AstNode parseExpression(const vector<Token>& tokens, int left, int right)
{
	//error.log("parsing expression: "+stringFromTokens(tokens, left, right), JSYK);
	
	if (left>right)
	{
		throw PineconeError(FUNC + " sent left higher then right", INTERNAL_ERROR, tokens[left]);
	}
	else if (left==right)
	{
		return AstToken::make(tokens[left]);
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
							return parseTokenList(tokens, left+1, right-1);
						else
							return AstVoid::make(); // a rare place where a astVoid may actually be intended by the programmer
					}
					else if (op==ops->openCrBrac)
					{
						return parseType(tokens, left+1, right-1);
					}
					else
					{
						throw PineconeError("unknown bracket '"+op->getText()+"'", INTERNAL_ERROR);
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
			AstNode leftNode=parseExpression(tokens, left, i-1);
			AstNode rightNode=parseExpression(tokens, i+1, right);
			
			if (tokens[i]->getOp()==ops->colon || tokens[i]->getOp()==ops->doubleColon)
			{
				return AstExpression::make(AstVoid::make(), move(leftNode), move(rightNode));
			}
			else
			{
				return AstExpression::make(move(leftNode), AstToken::make(tokens[i]), move(rightNode));
			}
		}
	}
	
	throw PineconeError("could not find where to split expression '" + stringFromTokens(tokens, left, right) + "'", SOURCE_ERROR, tokens[left]);
		
	//error.log("range: " + ([&]()->string{string out; for (int i=left; i<=right; i++) {out+=tokens[i]->getText()+" ";} return out;})(), JSYK, tokens[left]);
	//error.log("isMin: " + ([&]()->string{string out; for (auto i: isMin) {out+="\n"+to_string(i.first)+", "+to_string(i.second);} return out;})(), JSYK, tokens[left]);
}
*/

AstNode parseExpression(const vector<Token>& tokens, int left, int right)
{
	error.log(FUNC+" called on '"+stringFromTokens(tokens, left, right)+"'", JSYK);
	
	if (left>right)
	{
		throw PineconeError(FUNC + " sent left higher then right", INTERNAL_ERROR, tokens[left]);
	}
	else if (left==right)
	{
		return AstToken::make(tokens[left]);
	}
	
	int minPrece=-1;
	int indexOfMin=-1;
	
	for (int i=left; i<=right; i++)
	{
		if (tokens[i]->getOp()==ops->openPeren || tokens[i]->getOp()==ops->openCrBrac)
		{
			int j=skipBrace(tokens, i);
			
			if (i==left && j==right)
			{
				if (tokens[i]->getOp()==ops->openPeren)
				{
					return parseTokenList(tokens, left+1, right-1);
				}
				else 
				{
					return parseType(tokens, left+1, right-1);
				}
			}
			
			i=j;
		}
		else if (tokens[i]->getOp() && (minPrece<0 || tokens[i]->getOp()->getPrece()<minPrece))
		{
			minPrece=tokens[i]->getOp()->getPrece();
			indexOfMin=i;
			
			if (minPrece%2)
			{
				minPrece++;
			}
		}
	}
	
	if (indexOfMin<0)
	{
		throw PineconeError(FUNC+" could not find operator to split expression", INTERNAL_ERROR, tokens[left]);
	}
	
	if (tokens[indexOfMin]->getOp()==ops->colon || tokens[indexOfMin]->getOp()==ops->doubleColon)
	{
		if (indexOfMin==left || indexOfMin==right)
		{
			throw PineconeError("improper use of '"+tokens[indexOfMin]->getOp()->getText()+"'", SOURCE_ERROR, tokens[indexOfMin]);
		}
		
		AstNode leftNode=parseExpression(tokens, left, indexOfMin-1);
		AstNode rightNode=parseExpression(tokens, indexOfMin+1, right);
		
		return AstExpression::make(AstVoid::make(), move(leftNode), move(rightNode));
	}
	else
	{
		AstNode leftNode=indexOfMin>left?parseExpression(tokens, left, indexOfMin-1):AstVoid::make();
		AstNode centerNode=AstToken::make(tokens[indexOfMin]);
		AstNode rightNode=indexOfMin<right?parseExpression(tokens, indexOfMin+1, right):AstVoid::make();
		
		return AstExpression::make(move(leftNode), move(centerNode), move(rightNode));
	}
}

AstNode parseTokenList(const vector<Token>& tokens, int left, int right)
{
	vector<AstNode> nodes;
	
	int start=left;
	int i=start;
	
	while (i<=right)
	{
		bool tokenTakesRightInput=(tokens[i]->getOp() && tokens[i]->getOp()->takesRightInput());
		int next=i+1;
		bool nextTokenTakesLeftInput=(next<=right && tokens[next]->getOp() && tokens[next]->getOp()->takesLeftInput());
		
		if (i==right || (!tokenTakesRightInput && !nextTokenTakesLeftInput))
		{
			error.log("yes", JSYK);
			
			try
			{
				AstNode node=parseExpression(tokens, start, i);
				nodes.push_back(move(node));
			}
			catch (PineconeError err)
			{
				err.log();
			}
			
			start=next;
		}
		
		i++;
	}
	
	if (nodes.size()==0)
	{
		return AstVoid::make();
	}
	else if (nodes.size()==1)
	{
		return move(nodes[0]);
	}
	else
	{
		return AstList::make(nodes);
	}
}

/*AstNode parseTokenList(const vector<Token>& tokens, int left, int right)
{
	vector<AstNode> nodes;
	
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
				if (nodes.empty())
				{
					return parseExpression(tokens, left, right);
				}
				else
				{
					try
					{
						nodes.push_back(parseExpression(tokens, left, right));
					}
					catch (PineconeError err)
					{
						err.log();
					}
					
					break;
				}
			}
			else if (!tokens[i]->getOp() && !tokens[i+1]->getOp())// if the left can't absorb the right and the right cant absorbe the left
			{
				nodes.push_back(parseExpression(tokens, left, i));
				break;
			}
			
			i++;
		}
		
		left=i+1;
	}
	
	return AstList::make(nodes);
}*/


void parseChain(const vector<Token>& tokens, int left, int right, vector<AstNode>& out)
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
	
	out.push_back(parseExpression(tokens, left, i-1));
	
	if (i<right)
		parseChain(tokens, i+1, right, out);
}

unique_ptr<AstType> parseType(const vector<Token>& tokens, int left, int right)
{
	vector<AstTupleType::NamedType> types;
	
	while (left<=right)
	{
		//	skip commas
		while (left<=right && tokens[left]->getOp()==ops->comma)
		{
			left++;
		}
		
		//	if this is a named subtype
		if (left+2<=right && tokens[left+1]->getOp()==ops->colon)
		{
			if (tokens[left]->getType()!=TokenData::IDENTIFIER)
			{
				throw PineconeError("identifier must be to the left of ':' in type", SOURCE_ERROR, tokens[left]);
			}
			
			Token name=tokens[left];
			unique_ptr<AstType> type;
			
			if (tokens[left+2]->getType()==TokenData::IDENTIFIER)
			{
				type=AstTokenType::make(tokens[left+2]);
				left+=3;
			}
			else if (tokens[left+2]->getOp()==ops->openCrBrac)
			{
				int j=skipBrace(tokens, left+2);
				
				if (j>right)
				{
					throw PineconeError(FUNC+" skipping brance went outside of range", INTERNAL_ERROR, tokens[left+1]);
				}
				
				type=parseType(tokens, left+2+1, j-1);
				left=j+1;
			}
			else
			{
				throw PineconeError("invalid thingy '"+tokens[left+2]->getText()+"' in type", SOURCE_ERROR, tokens[left+2]);
			}
			
			types.push_back(AstTupleType::NamedType{name, move(type)});
		}
		else //	this is an unnamed subtype
		{
			types.push_back(AstTupleType::NamedType{nullptr, AstTokenType::make(tokens[left])});
			left+=1;
		}
	}
	
	if (types.size()==0)
		return AstVoidType::make();
	else if (types.size()==1 && !types[0].name)
		return move(types[0].type);
	else
		return AstTupleType::make(types);
}

/* Type parseTypeToken(Token token, Namespace table)
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
		error.log(FUNC+" called with non identifier token "+token->getText(), INTERNAL_ERROR, token);
		return Void;
	}
}
*/


