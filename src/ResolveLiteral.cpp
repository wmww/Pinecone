#include "../h/Action.h"
#include "../h/Token.h"
#include "../h/ErrorHandler.h"

Action resolveLiteral(Token token)
{
	if (token->getType()!=TokenData::LITERAL)
	{
		throw PineconeError(FUNC+" called on token that is not a literal", INTERNAL_ERROR, token);
	}
	
	string in=token->getText();
	
	if (in.empty())
		return nullptr;
	
	//bool floatingPoint=false;
	
	Type type=Unknown;
	
	if (in.empty())
	{
		error.log("tried to make literal with empty string", INTERNAL_ERROR, token);
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
				error.log(string() + "bad character '" + *i + "' found in number '" + in + "'", SOURCE_ERROR, token);
				return nullptr;
			}
			
			val=val*10+(*i-'0');
		}
		
		if (type==Bool)
		{
			bool out=(val!=0);
			return constGetAction(&out, type, token->getText());
		}
		else
		{
			int out=val;
			return constGetAction(&out, type, token->getText());
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
					error.log(string() + "multiple decimal points found in number '" + in + "'", SOURCE_ERROR, token);
					return voidAction;
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
				error.log(string() + "bad character '" + *i + "' found in number '" + in + "'", SOURCE_ERROR, token);
				return voidAction;
			}
		}
		
		double out=val;
		return constGetAction(&out, type, token->getText());
	}
	else
	{
		error.log("tried to make literal with invalid type of " + type->getString(), INTERNAL_ERROR, token);
		return voidAction;
	}
}
