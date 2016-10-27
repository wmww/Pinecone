
#include "../h/OperatorElement.h"
#include "../h/ErrorHandler.h"
#include "../h/IdentifierElement.h"
#include "../h/CastElement.h"
#include "../h/BranchAction.h"


ElementPtr OperatorElement::makeNew(ElementData dataIn)
{
	vector<Operator> ops;
	
	getOperators(ops, dataIn.text, dataIn);
	
	if (ops.size()==1)
	{
		return ElementPtr(new OperatorElement(dataIn, ops[0]));
	}
	else
	{
		error.log("operator error for '" + dataIn.text + "', number of returned operators is " + to_string(ops.size()), dataIn, SOURCE_ERROR);
		return ElementPtr(nullptr);
	}
}

string OperatorElement::getReadableName()
{
	return op->getText();
}

ActionPtr OperatorElement::resolveActions(ActionTablePtr table)
{
	ActionPtr leftAction, rightAction, out=nullptr;
	
	if (rightInput)
		rightAction=rightInput->resolveActions(table);
	else
		rightAction=voidAction;
	
	if (op==opColon)
	{
		if (leftInput && leftInput->getElemType()==ElementData::IDENTIFIER)
		{
			out=((IdentifierElement *)&(*leftInput))->resolveActions(table, voidAction, rightAction);
			
			if (out==voidAction)
			{
				error.log("Element::resolveActions returned voidAction", data, INTERNAL_ERROR);
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
			leftAction=voidAction;
			
		/*ActionPtr action=table->getBestAction(op, leftAction->getReturnType(), rightAction->getReturnType());
		ActionPtr action=table->getBestAction(op, leftAction->getReturnType(), rightAction->getReturnType());
		
		if (!action)
		{
			action=voidAction;
		}
		
		out=ActionPtr(new BranchAction(leftAction, action, rightAction));
		*/
		
		out=table->makeBranchAction(data, op, leftAction, rightAction);
	}
	
	if (!out)
	{
		error.log("could not resolve " + leftAction->getReturnType()->getName() + op->getText() + rightAction->getReturnType()->getName(), data, SOURCE_ERROR);
		return voidAction;
	}
	else
	{
		return out;
	}
	
}

/*
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
*/

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

