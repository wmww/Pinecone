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

void lexString(shared_ptr<SourceFile> file, vector<Token>& tokens);

//	unless otherwise noted, these are what the perams for the following functions mean
//		tokens: the tokens to parse
//		table: the table to use
//		left: left most token to parse (inclusive)
//		right: right most token to parse (inclusive)
//		returns: (if type is AstNode) the action pointer for that section of the program

//	splits a stream of tokens into a ListAstNode and calls parseExpression on each expression
void parseTokenList(const vector<Token>& tokens, int left, int right, vector<AstNode>& nodes);

int findExpressionSplit(const vector<Token>& tokens, int left, int right);

//	recursivly parses a single expression (no action lists)
AstNode parseExpression(const vector<Token>& tokens, int left, int right);

//	returns the index of the closing brace that matches the given opening brace index, works with (), [], and {}
//		tokens: the token array to use
//		start: the index of an open peren
//		returns: the index of the close peren that matches
int skipBrace(const vector<Token>& tokens, int start);

void parseSequence(const vector<Token>& tokens, int left, int right, Operator splitter, vector<AstNode>& out);

AstNode parseOperator(const vector<Token>& tokens, int left, int right, int index);

//AstNode parseLiteral(Token token);

unique_ptr<AstType> parseType(const vector<Token>& tokens, int left, int right);
//AstNode parseSingleTypeElement(const vector<Token>& tokens, int& i, int right, string& name, Type& type);
//Type parseTypeToken(Token token);

void importFile(vector<AstNode>& nodes, string path);

//AstNode parseIdentifier(Token token, AstNode leftIn, AstNode rightIn);

//void parseIdentifierConst(Token token, AstNode rightIn);

AstNode astNodeFromTokens(const vector<Token>& tokens, int left, int right)
{
	vector<AstNode> nodes;
	
	parseTokenList(tokens, left, right, nodes);
	
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

int findExpressionSplit(const vector<Token>& tokens, int left, int right)
{
	int minPrece=-1;
	int indexOfMin=-1;
	
	for (int i=left; i<=right; i++)
	{
		//cout << "looking at " << tokens[i]->getText() << endl;
		/*if (tokens[i]->getOp())
		{
			//cout << "precedence: " << tokens[i]->getOp()->getPrecedence()
		}
		*/
		
		if (ops->isOpenBrac(tokens[i]->getOp()))
		{
			int j=skipBrace(tokens, i);
			
			i=j; // i is now the close brace, and after it is incremented it will be the token after that
		}
		else if (tokens[i]->getOp() && (minPrece<0 || tokens[i]->getOp()->getPrecedence()<minPrece))
		{
			minPrece=tokens[i]->getOp()->getPrecedence();
			indexOfMin=i;
			
			// this ensures that if the precedence is divisable by 2, the same precedence again will replace this one as the min
			// it is my way of making even precedences be right associative and odd ones be left associative
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
	
	return indexOfMin;
}

AstNode parseExpression(const vector<Token>& tokens, int left, int right)
{
	//error.log(FUNC+" called on '"+stringFromTokens(tokens, left, right)+"'", JSYK);
	
	if (left>right)
	{
		throw PineconeError(FUNC + " sent left higher then right", INTERNAL_ERROR, tokens[left]);
	}
	else if (left==right)
	{
		return AstToken::make(tokens[left]);
	}
	
	if (ops->isOpenBrac(tokens[left]->getOp()) && skipBrace(tokens, left)==right) // if the braces enclose this entire expression
	{
		if (tokens[left]->getOp()==ops->openPeren)
		{
			return astNodeFromTokens(tokens, left+1, right-1);
		}
		else if (tokens[left]->getOp()==ops->openCrBrac)
		{
			return parseType(tokens, left+1, right-1);
		}
		else
		{
			throw PineconeError("unhandled brace '"+tokens[left]->getOp()->getText()+"'", INTERNAL_ERROR, tokens[left]);
		}
	}
	
	int i=findExpressionSplit(tokens, left, right);
	
	Operator op=tokens[i]->getOp();
	
	if (op==ops->minus && i==left && i!=right)
	{
		//dont error
	}
	else if ((i==left)==op->takesLeftInput() || (i==right)==op->takesRightInput())
	{
		throw PineconeError("improper use of '"+op->getText()+"' operator", SOURCE_ERROR, tokens[i]);
	}
	
	if (op==ops->pipe)
	{
		throw PineconeError("invalid use of '"+op->getText()+"'", SOURCE_ERROR, tokens[i]);
	}
	else if (op==ops->ifOp || op==ops->loop || op==ops->rightArrow || op==ops->andOp || op==ops->orOp)
	{
		vector<AstNode> leftNodes;
		vector<AstNode> rightNodes;
		
		if (i>left)
			parseSequence(tokens, left, i-1, ops->pipe, leftNodes);
		
		if (i<right)
			parseSequence(tokens, i+1, right, ops->pipe, rightNodes);
		
		return AstOpWithInput::make(leftNodes, tokens[i], rightNodes);
	}
	else if (op==ops->comma)
	{
		vector<AstNode> nodes;
		
		parseSequence(tokens, left, right, ops->comma, nodes);
		
		return AstTuple::make(nodes);
	}
	else if (op==ops->doubleColon)
	{
		unique_ptr<AstToken> centerNode=nullptr;
		AstNode rightNode=nullptr;
		
		if (i==left+1 && tokens[i-1]->getType()==TokenData::IDENTIFIER)
		{
			centerNode=AstToken::make(tokens[i-1]);
		}
		else
		{
			throw PineconeError("you can only use constant assignment on a single identifier", SOURCE_ERROR, tokens[i]);
		}
		
		if (i<right)
		{
			rightNode=parseExpression(tokens, i+1, right);
		}
		else
		{
			rightNode=AstVoid::make();
		}
		
		return AstConstExpression::make(move(centerNode), move(rightNode));
	}
	else if (op==ops->notEqual)
	{
		AstNode rightNode=i<right?parseExpression(tokens, i+1, right):AstVoid::make();
		AstNode leftNode=i>left?parseExpression(tokens, left, i-1):AstVoid::make();
		AstNode centerNode=AstToken::make(
			makeToken(
				ops->equal->getText(),
				tokens[i]->getFile(),
				tokens[i]->getLine(),
				tokens[i]->getCharPos()+1,
				TokenData::OPERATOR,
				ops->equal
			)
		);
		
		AstNode notNode=AstToken::make(
			makeToken(
				ops->notOp->getText(),
				tokens[i]->getFile(),
				tokens[i]->getLine(),
				tokens[i]->getCharPos(),
				TokenData::OPERATOR,
				ops->notOp
			)
		);
		
		return AstExpression::make(
			AstVoid::make(),
			move(notNode),
			AstExpression::make(
				move(leftNode),
				move(centerNode),
				move(rightNode)
			)
		);
	}
	else if (op==ops->plusPlus || op==ops->minusMinus)
	{
		throw PineconeError("++ and -- are not yet implemented", SOURCE_ERROR, tokens[i]);
		
		//AstNode leftNode=parseExpression(tokens, left, i-1);
		
		//return AstExpression::make(AstVoid::make(), leftNode, AstExpression::make(leftNode, AstToken::make(Token)))
	}
	else if (op==ops->dot)
	{
		return AstExpression::make
		(
			i>left ? parseExpression(tokens, left, i-1) : AstVoid::make(),
			i<right ? parseExpression(tokens, i+1, right) : AstVoid::make(),
			AstVoid::make()
		);
	}
	else if (op==ops->colon)
	{
		AstNode leftNode=AstVoid::make();
		AstNode centerNode=parseExpression(tokens, left, i-1);;
		AstNode rightNode=i<right?parseExpression(tokens, i+1, right):AstVoid::make();
		
		// make sure if it is an abc.xyz: ijk structure, it gets parsed as such, rather then (abc.xyz): ijk
		// we do this by parsing it the latter way (already done) and then detecting if we need to change it
		if (typeid(*centerNode)==typeid(AstExpression))
		{
			AstExpression * exprNode=(AstExpression*)&*centerNode;
			
			if (!exprNode->leftIn->isVoid() && !exprNode->center->isVoid() && exprNode->rightIn->isVoid())
			{
				leftNode=move(exprNode->leftIn);
				centerNode=move(exprNode->center);
			}
		}
		
		// make a function body if needed, else make a normal expression
		
		if (
			(centerNode->isType() || centerNode->isFunctionWithOutput())
			&&
			(leftNode->isVoid() || leftNode->isType())
			&&
			!rightNode->isType()
		){
			if (leftNode->isVoid())
				leftNode=AstVoidType::make();
			
			AstNode funcRightIn;
			AstNode funcReturn;
			
			if (centerNode->isFunctionWithOutput())
			{
				funcRightIn=move(((AstOpWithInput*)&*(centerNode))->leftIn[0]);
				funcReturn=move(((AstOpWithInput*)&*(centerNode))->rightIn[0]);
			}
			else
			{
				funcRightIn=move(centerNode);
				funcReturn=AstVoidType::make();
			}
			
			return AstFuncBody::make(move(leftNode), move(funcRightIn), move(funcReturn), move(rightNode));
		}
		else
		{
			return AstExpression::make(move(leftNode), move(centerNode), move(rightNode));
		}
	}
	else
	{
		return AstExpression::make
		(
			i>left ? parseExpression(tokens, left, i-1) : AstVoid::make(),
			AstToken::make(tokens[i]),
			i<right ? parseExpression(tokens, i+1, right) : AstVoid::make()
		);
	}
}

void parseTokenList(const vector<Token>& tokens, int left, int right, vector<AstNode>& nodes)
{
	int start=left;
	
	for (int i=left; i<=right; i++)
	{
		if (tokens[i]->getOp()==ops->import)
		{
			if (i==right || tokens[i+1]->getType()!=TokenData::STRING_LITERAL)
			{
				throw PineconeError("'"+ops->import->getText()+"' must be followed by a string literal", SOURCE_ERROR, tokens[i]);
			}
			
			string path=tokens[i+1]->getText();
			
			//this nonesens is required because my lexer is shit and includes the first quote but not the last one
			//instead of hardcoding that in, I figured I'd make it flexable so I don't break everthing when I fix the lexer
			
			while (path.size()>0 && path[0]=='"')
				path=path.substr(1, string::npos);
			
			while (path.size()>0 && path[path.size()-1]=='"')
				path=path.substr(0, path.size()-1);
			
			path=tokens[i]->getFile()->getDirPath()+"/"+path;
			
			try
			{
				importFile(nodes, path);
			}
			catch (PineconeError err)
			{
				err.log();
			}
			
			
			i+=2;
			start=i;
		}
		else
		{
			if (ops->isOpenBrac(tokens[i]->getOp()))
			{
				i=skipBrace(tokens, i);
			}
			
			bool tokenTakesRightInput=(tokens[i]->getOp() && tokens[i]->getOp()->takesRightInput());
			int next=i+1;
			bool nextTokenTakesLeftInput=(next<=right && tokens[next]->getOp() && tokens[next]->getOp()->takesLeftInput());
			
			if (i==right || (!tokenTakesRightInput && !nextTokenTakesLeftInput))
			{
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
		}
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

void parseSequence(const vector<Token>& tokens, int left, int right, Operator splitter, vector<AstNode>& out)
{
	int start=left;
	
	for (int i=left; i<=right; i++)
	{
		if (ops->isOpenBrac(tokens[i]->getOp()))
		{
			i=skipBrace(tokens, i);
		}
		else if (tokens[i]->getOp()==splitter)
		{
			if (start<=i-1)
				out.push_back(parseExpression(tokens, start, i-1));
			start=i+1;
		}
		else if (tokens[i]->getOp() && tokens[i]->getOp()->getPrecedence()==splitter->getPrecedence())
		{
			break;
		}
		else if (tokens[i]->getOp() && tokens[i]->getOp()->getPrecedence()<splitter->getPrecedence())
		{
			out.clear();
			out.push_back(parseExpression(tokens, left, right));
			return;
		}
	}
	
	if (start<=right)
		out.push_back(parseExpression(tokens, start, right));
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

void importFile(vector<AstNode>& nodes, string path)
{
	auto file=shared_ptr<SourceFile>(new SourceFile(path, false));
	
	if (file->getContents().empty())
	{
		throw PineconeError("file '"+path+"' failed to open or was empty", SOURCE_ERROR);
	}
	
	vector<Token> tokens;
	
	lexString(file, tokens);
	
	parseTokenList(tokens, 0, tokens.size()-1, nodes);
}

