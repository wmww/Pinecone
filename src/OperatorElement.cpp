
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
	ActionPtr leftAction, rightAction, out=nullptr;
	
	if (rightInput)
		rightAction=rightInput->resolveActions(table);
	else
		rightAction=voidAction;
	
	if (opType==OP_COLON)
	{
		if (leftInput && leftInput->getElemType()==ElementData::IDENTIFIER)
		{
			ActionPtr action=((IdentifierElement *)&(*leftInput))->resolveActions(table, Void, rightAction->getReturnType());
			
			if (!action)
			{
				error.log("Element::resolveActions returned a nullptr, this should never happen, use a VoidAction", data, INTERNAL_ERROR);
				action=voidAction;
			}
			
			out=ActionPtr(new BranchAction(voidAction, action, rightAction));
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
			leftAction=voidAction;
			
		ActionPtr action=table->getBestAction(opType, leftAction->getReturnType(), rightAction->getReturnType());
		
		if (!action)
		{
			action=voidAction;
		}
		
		out=ActionPtr(new BranchAction(leftAction, action, rightAction));
	}
	
	if (!out)
	{
		error.log("could not resolve " + leftAction->getReturnType()->getName() + toString(opType) + rightAction->getReturnType()->getName(), data, SOURCE_ERROR);
		return voidAction;
	}
	else
	{
		return out;
	}
	
}

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

