#include "../h/LiteralElement.h"
#include "../h/msclStringFuncs.h"
#include "../h/ErrorHandler.h"
#include "../h/Type.h"

ElementPtr LiteralElement::makeNew(ElementData dataIn)
{
	string& in=dataIn.text;
	
	if (in.empty())
		return nullptr;
	
	//bool floatingPoint=false;
	
	Type type=UnknownType;
	
	if (in.empty())
	{
		error.log("tried to make literal with empty string", dataIn, INTERNAL_ERROR);
	}
	
	if ((in[0]>='0' && in[0]<='9') || in[0]=='.')
	{
		type=Int;
		
		for (auto i=in.begin(); i!=in.end(); ++i)
		{
			if (*i=='.' || *i=='_')
			{
				type=Dub;
				break;
			}
		}
		
		if (in.back()=='d' || in.back()=='f')
		{
			type=Dub;
			in.pop_back();
		}
		else if (in.back()=='i')
		{
			type=Int;
			in.pop_back();
		}
		else if (in.back()=='b')
		{
			type=Bool;
			in.pop_back();
		}
	}
	
	if (type==Int || type==Int)
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
		
		if (type==Bool)
		{
			bool out=(val!=0);
			return ElementPtr(new LiteralElement(&out, type, dataIn));
		}
		else
		{
			int out=val;
			return ElementPtr(new LiteralElement(&out, type, dataIn));
		}
	}
	else if (type==Dub) //floating point
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
		
		double out=val;
		return ElementPtr(new LiteralElement(&out, type, dataIn));
	}
	else
	{
		error.log("tried to make literal with invalid type of " + type->toString(), dataIn, INTERNAL_ERROR);
		return nullptr;
	}
}

ActionPtr LiteralElement::resolveActions(ActionTablePtr table)
{
	if (type && val)
		return ActionPtr(new ConstGetAction(val, type, data.text));
	else
	{
		error.log(string() + "called " + __FUNCTION__ + " on a LiteralElement that was not properly created", data, INTERNAL_ERROR);
		return voidAction;
	}
}
