
#include "../h/IdentifierElement.h"
#include "../h/ErrorHandler.h"
#include "../h/StackFrame.h"

#include <string>
using std::to_string;

ElementPtr IdentifierElement::makeNew(ElementData dataIn)
{
	return ElementPtr(new IdentifierElement(dataIn));
}

string IdentifierElement::getReadableName()
{
	return data.text + " (identifier)";
}

ActionPtr IdentifierElement::resolveActions(ActionTablePtr table, Type leftType, Type rightType)
{
	if (!leftType || !rightType)
	{
		error.log("IdentifierElement::resolveActions sent null type", data, INTERNAL_ERROR);
	}
	
	
	ActionPtr out=table->getBestAction(data, leftType, rightType);
	
	if (!out)
	{
		if (rightType->isCreatable() && leftType->matches(Void))
		{
			size_t offset=table->getStackFrame()->getSize();
			table->getStackFrame()->addMember(rightType);
			
			ActionPtr getAction(new VarGetAction(offset, rightType, data.text));
			ActionPtr setAction(new VarSetAction(offset, rightType, data.text));
			out = setAction;
			table->addAction(getAction);
			table->addAction(setAction);
		}
		else
		{
			error.log(string() + "could not find proper overload for " + leftType->getName() + "." + data.text + ":" + rightType->getName(), data, SOURCE_ERROR);
			out=voidAction;
		}
	}
	
	return out;
}
