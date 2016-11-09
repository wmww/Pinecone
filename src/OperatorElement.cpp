
#include "../h/OperatorElement.h"
#include "../h/ErrorHandler.h"
#include "../h/IdentifierElement.h"
#include "../h/CastElement.h"


ElementPtr OperatorElement::makeNew(ElementData dataIn)
{
	vector<Operator> ops;
	
	getOperators(dataIn.text, ops);
	
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
	ActionPtr out=nullptr;
	 
	if (op==opColon)
	{
		ActionPtr rightAction=voidAction;
		
		if (rightInput)
			rightAction=rightInput->resolveActions(table);
		
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
	else if (op==opIf)
	{
		ActionPtr leftAction=voidAction;
		ActionPtr rightAction=voidAction;
		
		if (leftInput)
			leftAction=leftInput->resolveActions(table);
		
		if (rightInput)
			rightAction=rightInput->resolveActions(table);
		
		leftAction=table->addConverter(leftAction, Bool);
		
		if (leftAction!=voidAction)
		{
			out=ifAction(leftAction, rightAction);
		}
	}
	else if (op==opLoop)
	{
		ActionPtr leftAction=voidAction;
		ActionPtr rightAction=voidAction;
		
		if (leftInput)
			leftAction=leftInput->resolveActions(table);
		
		if (rightInput)
			rightAction=rightInput->resolveActions(table);
		
		leftAction=table->addConverter(leftAction, Bool);
		
		if (leftAction!=voidAction)
		{
			out=loopAction(leftAction, rightAction);
		}
	}
	else
	{
		ActionPtr leftAction=voidAction;
		ActionPtr rightAction=voidAction;
		
		if (leftInput)
			leftAction=leftInput->resolveActions(table);
		
		if (rightInput)
			rightAction=rightInput->resolveActions(table);
			
		/*ActionPtr action=table->getBestAction(op, leftAction->getReturnType(), rightAction->getReturnType());
		ActionPtr action=table->getBestAction(op, leftAction->getReturnType(), rightAction->getReturnType());
		
		if (!action)
		{
			action=voidAction;
		}
		
		out=ActionPtr(new BranchAction(leftAction, action, rightAction));
		*/
		
		out=table->makeBranchAction(data, op, leftAction, rightAction);
		
		if (!out)
			error.log("could not resolve " + leftAction->getReturnType()->getName() + op->getText() + rightAction->getReturnType()->getName(), data, SOURCE_ERROR);
	}
	
	if (!out)
	{
		error.log("could not resolve " + op->getText(), data, SOURCE_ERROR);
		return voidAction;
	}
	else
	{
		return out;
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

