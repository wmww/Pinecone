
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
		return action->getName() + " (" + action->getInLeftType().toString() + "." + action->getReturnType().toString() + ": " + action->getInRightType().toString() + ")";
	else
		return data.text + " [unresolved]";
}

void IdentifierElement::resolveIdentifiers(IdentifierTable& table, Type leftType, Type rightType)
{
	Identifier * ptr=table.getIdentifier(data.text);
	
	if (!ptr)
	{
		if (rightType.isCreatable() && leftType.exactlyEquals(Type(Type::VOID)))
		{
			ptr=table.makeIdentifier(data.text);
			ptr->addAction(new VarGetAction(DataElem::makeNewOfType(rightType), data.text));
			action=new VarSetAction(DataElem::makeNewOfType(rightType), data.text);
			ptr->addAction(action);
		}
		else
		{
			error.log("cannot create identifier " + data.text, data, SOURCE_ERROR);
		}
	}
	else
	{
		for (auto i=ptr->getActions().begin(); i!=ptr->getActions().end(); ++i)
		{
			if (
				leftType.exactlyEquals((*i)->getInLeftType()) &&
				rightType.exactlyEquals((*i)->getInRightType()))
			{
				action=*i;
				break;;
			}
		}
	}
	
	if (!action)
	{
		error.log(string() + "could not find proper overload for " + data.text, data, SOURCE_ERROR);
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

DataElem * IdentifierElement::execute()
{
	if (action && action->getInLeftType().isVoid() && action->getInRightType().isVoid())
	{
		DataElem * in=new VoidData();
		DataElem * out=execute(in, in);
		delete in;
		return out;
	}
	else
	{
		error.log(string() + "bad call of " + __FUNCTION__, data, INTERNAL_ERROR);
		return DataElem::makeNewOfType(getReturnType());
	}
}

DataElem * IdentifierElement::execute(DataElem * left, DataElem * right)
{
	if (action)
	{
		return action->execute(left, right);
	}
	else
	{
		return DataElem::makeNewOfType(getReturnType());
	}
}

