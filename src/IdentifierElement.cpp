
#include "../h/IdentifierElement.h"
#include "../h/ErrorHandler.h"
#include "../h/StackFrame.h"
#include "../h/Action.h"

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

ActionPtr IdentifierElement::resolveActions(ActionTablePtr table, ActionPtr leftAction, ActionPtr rightAction)
{
	ActionPtr out=table->makeBranchAction(data, leftAction, rightAction);
	
	if (out==voidAction)
	{
		Type type=rightAction->getReturnType();
		
		if (type->isCreatable() && leftAction==voidAction)
		{
			size_t offset=table->getStackFrame()->getSize();
			table->getStackFrame()->addMember(type);
			
			ActionPtr getAction=varGetAction(offset, type, data.text);
			ActionPtr setAction=varSetAction(offset, type, data.text);
			out = branchAction(voidAction, setAction, rightAction);
			table->addAction(getAction);
			table->addAction(setAction);
		}
		else
		{
			error.log(string() + "could not find proper overload for " + leftAction->getReturnType()->getName() + "." + data.text + ":" + type->getName(), data, SOURCE_ERROR);
			out=voidAction;
		}
	}
	
	return out;
}
