
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

void OperatorElement::resolveIdentifiers(ActionTable& table)
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

void* OperatorElement::execute()
{
	if (opType==COLON)
	{
		if (leftInput && rightInput && leftInput->getElemType()==ElementData::IDENTIFIER)
		{
			void* right=rightInput->execute();
			void* out=((IdentifierElement*)leftInput)->execute(nullptr, right);
			rightInput->getReturnType().deleteVoidPtr(right);
			return out;
		}
		else
		{
			error.log("can only assign an identifier an input", data, INTERNAL_ERROR);
			return nullptr;
		}
	}
	else
	{
		void* leftIn=nullptr;
		void* rightIn=nullptr;
		void* out=nullptr;
		
		if (leftInput)
			leftIn=leftInput->execute();
		
		if (rightInput)
			rightIn=rightInput->execute();
		
		switch (returnType.getType())
		{
			case Type::BOOL:
			{
				bool left=*((bool *)leftIn);
				bool right=*((bool *)rightIn);
				
				switch (opType)
				{
					case PLUS:
						out=new bool(left || right);
						break;
					
					case MINUS:
						out=new bool(left!=right);
						break;
						
					default:
						error.log("problem executing unimplemented type " + toString(opType) + " with type " + getReturnType().toString(), data, INTERNAL_ERROR);
						break;
				}
			}
			break;
			
			case Type::INT:
			{
				int left=*((int *)leftIn);
				int right=*((int *)rightIn);
				
				switch (opType)
				{
					case PLUS:
						out=new int(left+right);
						break;
						
					case MINUS:
						out=new int(left-right);
						break;
						
					default:
						error.log("problem executing unimplemented type " + toString(opType) + " with type " + getReturnType().toString(), data, INTERNAL_ERROR);
						break;
						
				}
			}
			break;
			
			case Type::DUB:
			{
				double left=*((double *)leftIn);
				double right=*((double *)rightIn);
				
				switch (opType)
				{
					case PLUS:
						out=new double(left+right);
						break;
					
					case MINUS:
						out=new double(left-right);
						break;
						
					default:
						error.log("problem executing unimplemented operator " + toString(opType) + " with type " + getReturnType().toString(), data, INTERNAL_ERROR);
						break;
				}
			}
			break;
			
			default:
				error.log(string() + "problem using unimplemented type " + returnType.toString(), data, INTERNAL_ERROR);
				break;
		}
		
		leftInput->getReturnType().deleteVoidPtr(leftIn);
		rightInput->getReturnType().deleteVoidPtr(rightIn);
		
		return out;
	}
}

string OperatorElement::toString(OpType in)
{
	switch (in)
	{
		case DOT: return ".";
		case PLUS: return "+";
		case MINUS: return "-";
		case COLON: return ":";
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

