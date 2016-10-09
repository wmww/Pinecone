#include "../h/Type.h"
#include "../h/ErrorHandler.h"

Type::Type(vector<Type>& typesIn)
{
	type=STRUCT;
	types.insert(types.end(), std::make_move_iterator(typesIn.begin()), std::make_move_iterator(typesIn.end()));
}

string Type::toString(PrimitiveType in)
{
	switch (in)
	{
		//case NONE: return "NO_TYPE";
		case UNKNOWN: return "UNKNOWN_TYPE";
		case VOID: return "Void";
		case STRUCT: return "STRUCT";
		case BOOL: return "Bool";
		case INT: return "Int";
		case DUB: return "Dub";
		default: return "ERROR_GETTING_TYPE";
	}
}

string Type::toString()
{
	if (type==STRUCT)
	{
		if (types.empty())
			return "EMPTY_STRUCT";
		
		string out;
		
		out+="(";
		
		for (unsigned i=0; i<types.size(); ++i)
		{
			out+=types[i].toString();
			if (i<types.size()-1)
				out+=", ";
		}
		
		out+=")";
		
		return out;
	}
	else
	{
		return toString(type);
	}
}

bool Type::isCreatable()
{
	if (type==INT || type==DUB || type==BOOL)
	{
		return true;
	}
	else if (type==STRUCT)
	{
		for (auto i=types.begin(); i!=types.end(); ++i)
		{
			if (!(*i).isCreatable())
				return false;
			
			return true;
		}
	}
	else if (type==VOID || type==UNKNOWN)// || type==NONE)
	{
		return false;
	}
	else
	{
		error.log("Type::isCreatable not implemented for type " + toString(type), ElementData(), INTERNAL_ERROR);
	}
	
	return false;
}

Type Type::getDominant(Type a, Type b)
{
	if ((a.getType()==STRUCT) != (b.getType()==STRUCT))
	{
		return Type();
	}
	else if (a.getType()==STRUCT)
	{
		vector<Type> av=a.getTypes();
		vector<Type> bv=b.getTypes();
		
		if (av.size()!=bv.size())
			return Type();
		
		vector<Type> types;
		
		for (unsigned i=0; i<av.size(); ++i)
		{
			types.push_back(getDominant(av[i], bv[i]));
		}
		
		return Type(types);
	}
	else
	{
		return Type(std::min(a.getType(), b.getType()));
	}
}

bool Type::operator==(const Type& other)
{
	/*if (type==NONE || other.type==NONE)
	{
		error.log("tried to compare a Type of type NONE, I think this is bad but I'm not really sure.", ElementData(), INTERNAL_ERROR);
		return false;
	}
	else */
	if (type==UNKNOWN || other.type==UNKNOWN)
	{
		return true;
	}
	else if (type==STRUCT && other.type==STRUCT)
	{
		for (unsigned i=0; i<types.size(); ++i)
		{
			if (!(types[i]==other.types[i]))
				return false;
		}
		
		return true;
	}
	else
	{
		return type==other.type;
	}
}

bool Type::exactlyEquals(const Type& other)
{
	if (type==STRUCT && other.type==STRUCT)
	{
		if (types.size()!=other.types.size())
			return false;
		
		for (unsigned i=0; i<types.size(); ++i)
		{
			if (!(types[i].exactlyEquals(other.types[i])))
				return false;
		}
		
		return true;
	}
	else
	{
		return type==other.type;
	}
}

