#include "../h/Type.h"
#include "../h/ErrorHandler.h"

const Type UnknownType = Type(new TypeData(TypeData::UNKNOWN, "UNKNOWN_TYPE"));
const Type Void = Type(new TypeData(TypeData::VOID, "Void"));
const Type Bool = Type(new TypeData(TypeData::BOOL, "Bool"));
const Type Int = Type(new TypeData(TypeData::INT, "Int"));
const Type Dub = Type(new TypeData(TypeData::DUB, "Dub"));

/*
TypeData::TypeData(shared_ptr<TypeData> typeIn, string nameIn)
{
	name=nameIn;
	type=typeIn->type;
	types=vector<Type>(typeIn->types);
}

TypeData::TypeData(PrimitiveType typeIn, string nameIn)
{
	type=typeIn;
	name=nameIn;
}

TypeData::TypeData(PrimitiveType shouldBeMetatype, Type typeIn, string nameIn)
{
	if (shouldBeMetatype==METATYPE)
	{
		type=METATYPE;
		name=nameIn;
		types.push_back(typeIn);
	}
	else
	{
		error.log("metatype created with primitive type that is not METATYPE", INTERNAL_ERROR);
		type=UNKNOWN;
		name="";
		types.push_back(Void);
	}
}

TypeData::TypeData(vector<shared_ptr<TypeData>> typesIn, string nameIn)
{
	name=nameIn;
	type=TUPLE;
	types.insert(types.end(), std::make_move_iterator(typesIn.begin()), std::make_move_iterator(typesIn.end()));
}

string TypeData::toString(PrimitiveType in)
{
	switch (in)
	{
		//case NONE: return "NO_TYPE";
		case UNKNOWN: return "UNKNOWN_TYPE";
		case VOID: return "VOID";
		case BOOL: return "BOOL";
		case INT: return "INT";
		case DUB: return "DUB";
		case TUPLE: return "TUPLE";
		case METATYPE: return "METATYPE";
		default: return "ERROR_GETTING_TYPE";
	}
}

string TypeData::toString()
{
	if (type==TUPLE)
	{
		if (types.empty())
			return "EMPTY_TUPLE";
		
		string out;
		
		out+="(";
		
		for (unsigned i=0; i<types.size(); ++i)
		{
			out+=types[i]->toString();
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

bool TypeData::isCreatable()
{
	if (type==INT || type==DUB || type==BOOL)
	{
		return true;
	}
	else if (type==TUPLE)
	{
		for (auto i=types.begin(); i!=types.end(); ++i)
		{
			if (!(*i)->isCreatable())
				return false;
			
			return true;
		}
	}
	else if (type==VOID || type==UNKNOWN || type==METATYPE)
	{
		return false;
	}
	else
	{
		error.log("Type::isCreatable not implemented for type " + toString(type), INTERNAL_ERROR);
	}
	
	return false;
}

bool TypeData::isVoid()
{
	return type==VOID;
}

size_t TypeData::getSize()
{
	switch (type)
	{
		case VOID: return 0;
		case METATYPE: return 0;
		case BOOL: return sizeof(bool);
		case INT: return sizeof(int);
		case DUB: return sizeof(double);
		
		case TUPLE:
		{
			size_t sum=0;
			for (unsigned i=0; i<types.size(); ++i)
				sum+=types[i]->getSize();
			return sum;
		}
			
		case UNKNOWN:
		default:
			error.log("tried to get size of " + toString(), INTERNAL_ERROR);
			return 0;
	}
}

bool TypeData::matches(Type other)
{
	if (type==TUPLE && other->type==TUPLE)
	{
		if (types.size()!=other->types.size())
			return false;
		
		for (unsigned i=0; i<types.size(); ++i)
		{
			if (!(types[i]->matches(other->types[i])))
				return false;
		}
		
		return true;
	}
	else
	{
		return type==other->type;
	}
}

Type newMetatype(Type typeIn)
{
	return Type(new TypeData(TypeData::METATYPE, typeIn, "{"+typeIn->getName()+"}"));
}
*/
