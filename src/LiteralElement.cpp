#include "../h/LiteralElement.h"
#include "../h/msclStringFuncs.h"
#include "../h/ErrorHandler.h"
#include "../h/Type.h"

LiteralElement * LiteralElement::makeNew(ElementData dataIn)
{
	string& in=dataIn.text;
	
	if (in.empty())
		return nullptr;
	
	//bool floatingPoint=false;
	
	Type::PrimitiveType type=Type::UNKNOWN;
	
	if (in.empty())
	{
		error.log("tried to make literal with empty string", dataIn, INTERNAL_ERROR);
	}
	
	if ((in[0]>='0' && in[0]<='9') || in[0]=='.')
	{
		type=Type::INT;
		
		for (auto i=in.begin(); i!=in.end(); ++i)
		{
			if (*i=='.' || *i=='_')
			{
				type=Type::DUB;
				break;
			}
		}
		
		if (in.back()=='d' || in.back()=='f')
		{
			type=Type::DUB;
			in.pop_back();
		}
		else if (in.back()=='i')
		{
			type=Type::INT;
			in.pop_back();
		}
		else if (in.back()=='b')
		{
			type=Type::BOOL;
			in.pop_back();
		}
	}
	
	if (type==Type::INT || type==Type::BOOL)
	{
		int val=0;
		
		for (auto i=in.begin(); i!=in.end(); ++i)
		{
			if (*i<'0' || *i>'9')
			{
				error.log(string() + "bad character '" + *i + "' found in number '" + in + "'", dataIn, SOURCE_ERROR);
				return nullptr;
			}
			
			val=val*10+(*i-'0');
		}
		
		if (type==Type::BOOL)
		{
			return new BoolLiteral(dataIn, val!=0);
		}
		else
		{
			return new IntLiteral(dataIn, val);
		}
	}
	else if (type==Type::DUB) //floating point
	{
		double val=0;
		int pointPos=0;
		
		for (auto i=in.begin(); i!=in.end(); ++i)
		{
			if (*i=='.' || *i=='_')
			{
				if (pointPos==0)
				{
					pointPos=10;
				}
				else
				{
					error.log(string() + "multiple decimal points found in number '" + in + "'", dataIn, SOURCE_ERROR);
					return nullptr;
				}
			}
			else if (*i>='0' && *i<='9')
			{
				if (pointPos)
				{
					val+=(double)(*i-'0')/pointPos;
					pointPos*=10;
				}
				else
				{
					val=val*10+(*i-'0');
				}
			}
			else
			{
				error.log(string() + "bad character '" + *i + "' found in number '" + in + "'", dataIn, SOURCE_ERROR);
				return nullptr;
			}
		}
		
		return new DubLiteral(dataIn, val);
	}
	else
	{
		error.log("tried to make literal with invalid type of " + Type::toString(type), dataIn, INTERNAL_ERROR);
		return nullptr;
	}
}

