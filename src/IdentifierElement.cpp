
#include "../h/IdentifierElement.h"
#include "../h/ErrorHandler.h"

#include <string>
using std::to_string;

IdentifierElement * IdentifierElement::makeNew(ElementData dataIn)
{
	return new IdentifierElement(dataIn);
}

string IdentifierElement::getReadableName()
{
	if (action)
		return action->getDescription() + " (" + action->getInLeftType().toString() + "." + action->getReturnType().toString() + ":" + action->getInRightType().toString() + ")";
	else
		return data.text + " [unresolved]";
}

void IdentifierElement::resolveIdentifiers(ActionTable& table, Type leftType, Type rightType)
{
	//Identifier * ptr=table.getIdentifier(data.text);
	action=table.getBestAction(data, leftType, rightType);
	
	if (!action)
	{
		if (rightType.isCreatable() && leftType.exactlyEquals(Type(Type::VOID)))
		{
			void* value = rightType.createVoidPtr();
			Action * getAction = new VarGetAction(value, rightType, data.text);
			Action * setAction = new VarSetAction(value, rightType, data.text);
			action = setAction;
			table.addAction(getAction);
			table.addAction(setAction);
		}
		else
		{
			error.log(string() + "could not find proper overload for " + data.text, data, SOURCE_ERROR);
		}
	}
}

Type IdentifierElement::getReturnType()
{
	if (action)
	{
		return action->getReturnType();
	}
	else
	{
		return Type();
	}
}

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

