
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
	if (rightInput)
		rightInput->resolveIdentifiers(table);
	
	if (opType==COLON)
	{
		if (leftInput->getElemType()==ElementData::IDENTIFIER)
		{
			((IdentifierElement *)leftInput)->resolveIdentifiers(table, Type(Type::VOID), rightInput->getReturnType());
			returnType=rightInput->getReturnType();
		}
		else
		{
			error.log("only an identifier can be assigned a value", data, SOURCE_ERROR);
		}
	}
	else
	{
		if (leftInput)
			leftInput->resolveIdentifiers(table);
		
		if (opType==PLUS || opType==MINUS)
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
}

DataElem * OperatorElement::execute()
{
	if (opType==COLON)
	{
		if (leftInput && rightInput && leftInput->getElemType()==ElementData::IDENTIFIER)
		{
			DataElem * right=rightInput->execute();
			DataElem * left=new VoidData();
			DataElem * out=((IdentifierElement *)leftInput)->execute(left, right);
			delete right;
			delete left;
			return out;
		}
		else
		{
			error.log("can only assign an identifier an input", data, INTERNAL_ERROR);
			return DataElem::makeNewOfType(returnType);
		}
	}
	else
	{
		DataElem * leftIn, * rightIn;
		
		if (leftInput)
			leftIn=leftInput->execute();
		else
			leftIn=new VoidData();
		
		if (rightInput)
			rightIn=rightInput->execute();
		else
			rightIn=new VoidData();
		
		
		switch (returnType.getType())
		{
			case Type::BOOL:
			{
				bool left=*((bool *)leftIn->getData());
				bool right=*((bool *)rightIn->getData());
				delete leftIn;
				delete rightIn;
				
				switch (opType)
				{
					case PLUS:
						return new BoolData(left || right);
					
					case MINUS:
						return new BoolData(left!=right);
						
					default:
						error.log("problem executing unimplemented type " + toString(opType), data, INTERNAL_ERROR);
						return new BoolData();
				}
			}
			
			case Type::INT:
			{
				int left=*((int *)leftIn->getData());
				int right=*((int *)rightIn->getData());
				delete leftIn;
				delete rightIn;
				
				switch (opType)
				{
					case PLUS:
						return new IntData(left+right);
					
					case MINUS:
						return new IntData(left+right);
						
					default:
						error.log("problem executing unimplemented type " + toString(opType), data, INTERNAL_ERROR);
						return new IntData();
				}
			}
			
			case Type::DUB:
			{
				double left=*((double *)leftIn->getData());
				double right=*((double *)rightIn->getData());
				delete leftIn;
				delete rightIn;
				
				switch (opType)
				{
					case PLUS:
						return new DubData(left || right);
					
					case MINUS:
						return new DubData(left!=right);
						
					default:
						error.log("problem executing unimplemented operator " + toString(opType), data, INTERNAL_ERROR);
						return new DubData();
				}
			}
			
			default:
				error.log(string() + "problem using unimplemented type " + returnType.toString(), data, INTERNAL_ERROR);
				return DataElem::makeNewOfType(returnType);
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

