
#include "../h/OperatorElement.h"
#include "../h/ErrorHandler.h"
#include "../h/IdentifierElement.h"
#include "../h/CastElement.h"

OperatorElement::~OperatorElement()
{
	if (leftInput)
		delete leftInput;
	
	if (rightInput)
		delete rightInput;
}

OperatorElement * OperatorElement::makeNew(ElementData dataIn)
{
	if (dataIn.text==".")
		return new OperatorElement(dataIn, DOT);
	
	if (dataIn.text==":")
		return new OperatorElement(dataIn, COLON);
	
	else if (dataIn.text=="+")
		return new OperatorElement(dataIn, PLUS);
		
	else if (dataIn.text=="-")
		return new OperatorElement(dataIn, MINUS);
	
	else if (dataIn.text=="(")
		return new OperatorElement(dataIn, OPEN);
	
	else if (dataIn.text==")")
		return new OperatorElement(dataIn, CLOSE);
	
	else
	{
		error.log("unknown operator '" + dataIn.text + "'", dataIn, SOURCE_ERROR);
		return nullptr;
	}
}

void OperatorElement::resolveIdentifiers(IdentifierTable& table)
{
	if (leftInput)
		leftInput->resolveIdentifiers(table);
	
	if (rightInput)
		rightInput->resolveIdentifiers(table);
	
	if (opType==COLON)
	{
		if (leftInput && rightInput && leftInput->getElemType()==ElementData::IDENTIFIER)
		{
			Identifier * id=((IdentifierElement *)(leftInput))->getIdentifier();
			
			if (id)
			{
				if (!id->getType().exactlyEquals(rightInput->getReturnType()))
				{
					rightInput=new CastElement(rightInput->getData(), id->getType(), rightInput);
				}
			}
			else
			{
				Type rightType=rightInput->getReturnType();
				
				if (rightType.isCreatable())
				{
					Identifier * ptr=table.getOrMakeIdentifier(leftInput->getData().text, rightType);
					
					if (ptr)
					{
						((IdentifierElement *)(leftInput))->setIdentifier(ptr);
					}
					else
					{
						error.log("cannot create identifier '" + leftInput->getData().text + "'",
									leftInput->getData(), SOURCE_ERROR);
					}
				}
				else
				{
					error.log("cannot create identifier '" + leftInput->getData().text + "' due to invalid type " + rightType.toString(),
								rightInput->getData(), SOURCE_ERROR);
				}
			}
		}
		
		returnType=leftInput->getReturnType();
	}
	else if (opType==PLUS || opType==MINUS)
	{
		if (leftInput && rightInput)
		{
			returnType=Type::getDominant(leftInput->getReturnType(), rightInput->getReturnType());
			
			if (!leftInput->getReturnType().exactlyEquals(returnType))
				leftInput=new CastElement(leftInput->getData(), returnType, leftInput);
			
			if (!rightInput->getReturnType().exactlyEquals(returnType))
				rightInput=new CastElement(rightInput->getData(), returnType, rightInput);
		}
	}
}

string OperatorElement::toString(OpType in)
{
	switch (in)
	{
		case DOT: return "_._";
		case PLUS: return "_+_";
		case MINUS: return "_-_";
		case COLON: return "_:_";
		case OPEN: return "(...";
		case CLOSE: return "...)";
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

