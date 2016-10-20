
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
	ActionPtr out=table->getBestAction(data, leftType, rightType);
	
	if (!out)
	{
		if (rightType.isCreatable() && leftType.exactlyEquals(Type(Type::VOID)))
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
			error.log(string() + "could not find proper overload for " + leftType.toString() + "." + data.text + ":" + rightType.toString(), data, SOURCE_ERROR);
		}
	}
	
	//returnType=out->getReturnType();
	
	return out;
}

/*Type IdentifierElement::getReturnType()
{
	return returnType;
}*/

/*
void* IdentifierElement::execute()
{
	return execute(nullptr, nullptr);
}

void* IdentifierElement::execute(void* left, void* right)
{
	if (action)
	{
		return action->execute(left, right);
	}
	else
	{
		error.log(string() + __FUNCTION__ + " called while action is null", data, INTERNAL_ERROR);
		return nullptr;
	}
}
*/
