#include "../h/Type.h"
#include "../h/ErrorHandler.h"

const Type UnknownType = Type(new TypeBase(TypeBase::UNKNOWN, "UNKNOWN_TYPE"));
const Type Void = Type(new TypeBase(TypeBase::VOID, "Void"));
const Type Bool = Type(new TypeBase(TypeBase::BOOL, "Bool"));
const Type Int = Type(new TypeBase(TypeBase::INT, "Int"));
const Type Dub = Type(new TypeBase(TypeBase::DUB, "Dub"));

TypeBase::TypeBase(vector<shared_ptr<TypeBase>>& typesIn, string nameIn)
{
	name=nameIn;
	type=STRUCT;
	types.insert(types.end(), std::make_move_iterator(typesIn.begin()), std::make_move_iterator(typesIn.end()));
}

string TypeBase::toString(PrimitiveType in)
{
	switch (in)
	{
		//case NONE: return "NO_TYPE";
		case UNKNOWN: return "UNKNOWN_TYPE";
		case VOID: return "VOID";
		case STRUCT: return "STRUCT";
		case BOOL: return "BOOL";
		case INT: return "INT";
		case DUB: return "DUB";
		default: return "ERROR_GETTING_TYPE";
	}
}

string TypeBase::toString()
{
	if (type==STRUCT)
	{
		if (types.empty())
			return "EMPTY_STRUCT";
		
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

bool TypeBase::isCreatable()
{
	if (type==INT || type==DUB || type==BOOL)
	{
		return true;
	}
	else if (type==STRUCT)
	{
		for (auto i=types.begin(); i!=types.end(); ++i)
		{
			if (!(*i)->isCreatable())
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
		error.log("Type::isCreatable not implemented for type " + toString(type), INTERNAL_ERROR);
	}
	
	return false;
}

bool TypeBase::isVoid()
{
	return type==VOID;
}

size_t TypeBase::getSize()
{
	switch (type)
	{
		case VOID: return 0;
		case BOOL: return sizeof(bool);
		case INT: return sizeof(int);
		case DUB: return sizeof(double);
		
		case STRUCT:
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

/*
Type TypeBase::getDominant(Type a, Type b)
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
*/

/*
bool TypeBase::operator==(const Type& other)
{
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

bool TypeBase::exactlyEquals(const Type& other)
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
*/

bool TypeBase::matches(Type other)
{
	if (type==STRUCT && other->type==STRUCT)
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

void* TypeBase::createVoidPtr()
{
	switch (type)
	{
		case BOOL: return new bool(false);
		case INT: return new int(0);
		case DUB: return new double(0.0);
		case VOID: return nullptr;
		
		case STRUCT:
		case UNKNOWN:
		default:
			error.log("tried to create uncreatable type " + toString(), INTERNAL_ERROR);
			return nullptr;
	}

}

void TypeBase::deleteVoidPtr(void* ptr)
{
	if (ptr)
	{
		switch (type)
		{
			case BOOL: delete ((bool*)ptr); break;
			case INT: delete ((int*)ptr); break;
			case DUB: delete ((double*)ptr); break;
			case VOID: break;
			
			case STRUCT:
			case UNKNOWN:
			default:
				error.log("tried to delete undeletable type " + toString(), INTERNAL_ERROR);
				break;
		}
	}
}

void TypeBase::setVoidPtr(void* ptr, void* in)
{
	if (ptr)
	{
		switch (type)
		{
			case BOOL: *((bool*)ptr)=*((bool*)in); break;
			case INT: *((int*)ptr)=*((int*)in); break;
			case DUB: *((double*)ptr)=*((double*)in); break;
			case VOID: break;
			
			case STRUCT:
			case UNKNOWN:
			default:
				error.log("tried to delete undeletable type " + toString(), INTERNAL_ERROR);
				break;
		}
	}
}

void* TypeBase::cloneVoidPtr(void* ptr)
{
	if (ptr)
	{
		switch (type)
		{
			case BOOL: return new bool(*((bool*)ptr));
			case INT: return new int(*((int*)ptr));
			case DUB: return new double(*((double*)ptr));
			case VOID: return nullptr;
			
			case STRUCT:
			case UNKNOWN:
			default:
				error.log("tried to clone unclonable type " + toString(), INTERNAL_ERROR);
				return nullptr;
		}
	}
	else
		return nullptr;
}

void* TypeBase::castVoidPtr(void* ptr, Type typeOut)
{
	if (ptr)
	{
		if (type==BOOL)
		{
			bool var=*((bool*)ptr);
			
			if (typeOut->matches(Bool))
				return new bool(var);
			else if (typeOut->matches(Int))
				return new int(var?1:0);
			else if (typeOut->matches(Dub))
				return new double(var?1.0:0.0);
			else
				return nullptr;
		}
		else if (type==INT)
		{
			int var=*((int*)ptr);
			
			if (typeOut->matches(Bool))
				return new bool(var!=0);
			else if (typeOut->matches(Int))
				return new int(var);
			else if (typeOut->matches(Dub))
				return new double((double)var);
			else
				return nullptr;
		}
		else if (type==DUB)
		{
			double var=*((double *)ptr);
			
			if (typeOut->matches(Bool))
				return new bool(var!=0);
			else if (typeOut->matches(Int))
				return new int((int)var);
			else if (typeOut->matches(Bool))
				return new double(var);
			else
				return nullptr;
		}
		else if (type==STRUCT)
		{
			return nullptr;
			
			/*//vector<DataElem *> var=*((vector<DataElem *>)ptr->getData());
			
			if (type==Type::BOOL)
				return new BoolData(var!=0);
			else if (type==Type::INT)
				return new IntData((int)var);
			else if (type==Type::DUB)
				return new DubData(var);
			else
				return new VoidData();
			
			delete ptr;*/
		}
		else if (type==VOID)
		{
			return nullptr;
		}
		else
		{
			error.log("unknown type " + toString() + " in CastElement::castToType", INTERNAL_ERROR);
			return nullptr;
		}
	}
	else
		return nullptr;
}

