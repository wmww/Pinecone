
#include "../h/OperatorElement.h"
#include "../h/ErrorHandler.h"
#include "../h/IdentifierElement.h"
#include "../h/CastElement.h"
#include "../h/BranchAction.h"

ElementPtr OperatorElement::makeNew(ElementData dataIn)
{
	if (dataIn.text==".")
		return ElementPtr(new OperatorElement(dataIn, OP_DOT));
	
	if (dataIn.text==":")
		return ElementPtr(new OperatorElement(dataIn, OP_COLON));
	
	else if (dataIn.text=="+")
		return ElementPtr(new OperatorElement(dataIn, OP_PLUS));
		
	else if (dataIn.text=="-")
		return ElementPtr(new OperatorElement(dataIn, OP_MINUS));
	
	else if (dataIn.text=="(")
		return ElementPtr(new OperatorElement(dataIn, OP_OPEN));
	
	else if (dataIn.text==")")
		return ElementPtr(new OperatorElement(dataIn, OP_CLOSE));
	
	else
	{
		error.log("unknown operator '" + dataIn.text + "'", dataIn, SOURCE_ERROR);
		return ElementPtr(nullptr);
	}
}

string OperatorElement::getReadableName()
{
	return toString(opType);
}

ActionPtr OperatorElement::resolveActions(ActionTablePtr table)
{
	ActionPtr leftAction, rightAction, out;
	
	if (rightInput)
		rightAction=rightInput->resolveActions(table);
	else
		rightAction=ActionPtr(new VoidAction());
	
	if (opType==OP_COLON)
	{
		if (leftInput && leftInput->getElemType()==ElementData::IDENTIFIER)
		{
			ActionPtr action=((IdentifierElement *)&(*leftInput))->resolveActions(table, Type(Type::VOID), rightAction->getReturnType());
			
			if (action)
			{
				out=ActionPtr(new BranchAction(ActionPtr(new VoidAction()), action, rightAction));
			}
		}
		else
		{
			error.log("only an identifier can be assigned a value", data, SOURCE_ERROR);
		}
	}
	else
	{
		if (leftInput)
			leftAction=leftInput->resolveActions(table);
		else
			leftAction=ActionPtr(new VoidAction());
			
		ActionPtr action=table->getBestAction(opType, leftAction->getReturnType(), rightAction->getReturnType());
		
		if (action)
		{
			out=ActionPtr(new BranchAction(leftAction, action, rightAction));
		}
	}
	
	if (!out)
	{
		error.log("could not resolve " + leftAction->getReturnType().toString() + toString(opType) + rightAction->getReturnType().toString(), data, SOURCE_ERROR);
		return ActionPtr(new VoidAction());
	}
	else
	{
		return out;
	}
	
	/*if (rightInput)
		rightInput->resolveActions(table);
	
	if (opType==COLON)
	{
		if (leftInput->getElemType()==ElementData::IDENTIFIER)
		{
			((IdentifierElement *)&(*leftInput))->resolveActions(table, Type(Type::VOID), rightInput->getReturnType());
		}
		else
		{
			error.log("only an identifier can be assigned a value", data, SOURCE_ERROR);
		}
	}
	else
	{
		if (leftInput)
			leftInput->resolveActions(table);
	}
	
	action=table->getBestAction(data, leftInput?leftInput->getReturnType():Type(), rightInput?rightInput->getReturnType():Type());
	
	if (!action)
	{
		error.log(string() + "could not find proper overload for " + data.text, data, SOURCE_ERROR);
	}
	else
	{
		if (leftInput && !leftInput->getReturnType().exactlyEquals(action->getReturnType()))
			leftInput=new CastElement(leftInput->getData(), action->getReturnType(), leftInput);
		
		if (rightInput && !rightInput->getReturnType().exactlyEquals(action->getReturnType()))
			rightInput=new CastElement(rightInput->getData(), action->getReturnType(), rightInput);
	}*/
	
}

/*
void* OperatorElement::execute()
{
	if (action)
	{
		void* leftData=leftInput?leftInput->execute():nullptr;
		void* rightDate=rightInput?rightInput->execute():nullptr;
		void* out=action->execute(leftData, rightDate);
		if (leftInput) leftInput->getReturnType().deleteVoidPtr(leftData);
		if (rightInput) rightInput->getReturnType().deleteVoidPtr(rightDate);
		return out;
	}
	else
	{
		error.log(string() + __FUNCTION__ + " called while action is null", data, INTERNAL_ERROR);
		return nullptr;
	}
	
	if (opType==COLON)
	{
		if (leftInput && rightInput && leftInput->getElemType()==ElementData::IDENTIFIER)
		{
			void* right=rightInput->execute();
			void* out=((IdentifierElement*)leftInput)->execute(nullptr, right);
			rightInput->getReturnType().deleteVoidPtr(right);
			return out;
		}
		else
		{
			error.log("can only assign an identifier an input", data, INTERNAL_ERROR);
			return nullptr;
		}
	}
	else
	{
		void* leftIn=nullptr;
		void* rightIn=nullptr;
		void* out=nullptr;
		
		if (leftInput)
			leftIn=leftInput->execute();
		
		if (rightInput)
			rightIn=rightInput->execute();
		
		switch (returnType.getType())
		{
			case Type::BOOL:
			{
				bool left=*((bool *)leftIn);
				bool right=*((bool *)rightIn);
				
				switch (opType)
				{
					case PLUS:
						out=new bool(left || right);
						break;
					
					case MINUS:
						out=new bool(left!=right);
						break;
						
					default:
						error.log("problem executing unimplemented type " + toString(opType) + " with type " + getReturnType().toString(), data, INTERNAL_ERROR);
						break;
				}
			}
			break;
			
			case Type::INT:
			{
				int left=*((int *)leftIn);
				int right=*((int *)rightIn);
				
				switch (opType)
				{
					case PLUS:
						out=new int(left+right);
						break;
						
					case MINUS:
						out=new int(left-right);
						break;
						
					default:
						error.log("problem executing unimplemented type " + toString(opType) + " with type " + getReturnType().toString(), data, INTERNAL_ERROR);
						break;
						
				}
			}
			break;
			
			case Type::DUB:
			{
				double left=*((double *)leftIn);
				double right=*((double *)rightIn);
				
				switch (opType)
				{
					case PLUS:
						out=new double(left+right);
						break;
					
					case MINUS:
						out=new double(left-right);
						break;
						
					default:
						error.log("problem executing unimplemented operator " + toString(opType) + " with type " + getReturnType().toString(), data, INTERNAL_ERROR);
						break;
				}
			}
			break;
			
			default:
				error.log(string() + "problem using unimplemented type " + returnType.toString(), data, INTERNAL_ERROR);
				break;
		}
		
		leftInput->getReturnType().deleteVoidPtr(leftIn);
		rightInput->getReturnType().deleteVoidPtr(rightIn);
		
		return out;
	}
}
*/

string OperatorElement::toString(OperatorType in)
{
	switch (in)
	{
		case OP_DOT: return ".";
		case OP_PLUS: return "+";
		case OP_MINUS: return "-";
		case OP_COLON: return ":";
		case OP_OPEN: return "(...";
		case OP_CLOSE: return "...)";
		default: return "UNKNOWN_OPERATOR";
	}
}

void OperatorElement::printToString(string& in, int depth)
{
	if (leftInput)
		leftInput->printToString(in, depth+1);
	
	printIndentationToString(in, depth);
	in+=getReadableName();
	in+="\n";
	
	if (rightInput)
		rightInput->printToString(in, depth+1);
}

