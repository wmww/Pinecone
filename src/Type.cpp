#include "../h/Type.h"
#include "../h/ErrorHandler.h"

const Type UnknownType = Type(new TypeData(TypeData::UNKNOWN, "UNKNOWN_TYPE"));
const Type Void = Type(new TypeData(TypeData::VOID, "Void"));
const Type Bool = Type(new TypeData(TypeData::BOOL, "Bool"));
const Type Int = Type(new TypeData(TypeData::INT, "Int"));
const Type Dub = Type(new TypeData(TypeData::DUB, "Dub"));

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

/*
Type TypeData::getDominant(Type a, Type b)
{
	if ((a.getType()==TUPLE) != (b.getType()==TUPLE))
	{
		return Type();
	}
	else if (a.getType()==TUPLE)
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
bool TypeData::operator==(const Type& other)
{
	if (type==UNKNOWN || other.type==UNKNOWN)
	{
		return true;
	}
	else if (type==TUPLE && other.type==TUPLE)
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

bool TypeData::exactlyEquals(const Type& other)
{
	if (type==TUPLE && other.type==TUPLE)
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

/*
void* TypeData::createVoidPtr()
{
	switch (type)
	{
		case BOOL: return new bool(false);
		case INT: return new int(0);
		case DUB: return new double(0.0);
		case VOID: return nullptr;
		
		case TUPLE:
		case UNKNOWN:
		default:
			error.log("tried to create uncreatable type " + toString(), INTERNAL_ERROR);
			return nullptr;
	}
	
	return malloc(getSize());
}

void TypeData::deleteVoidPtr(void* ptr)
{
	free(ptr);
	
	if (ptr)
	{
		switch (type)
		{
			case BOOL: delete ((bool*)ptr); break;
			case INT: delete ((int*)ptr); break;
			case DUB: delete ((double*)ptr); break;
			case VOID: break;
			
			case TUPLE:
			case UNKNOWN:
			default:
				error.log("tried to delete undeletable type " + toString(), INTERNAL_ERROR);
				break;
		}
	}
}

void TypeData::setVoidPtr(void* ptr, void* val)
{
	memcpy(ptr, val, getSize());
	
	
	if (ptr)
	{
		switch (type)
		{
			case BOOL: *((bool*)ptr)=*((bool*)val); break;
			case INT: *((int*)ptr)=*((int*)val); break;
			case DUB: *((double*)ptr)=*((double*)val); break;
			case VOID: break;
			
			case TUPLE:
			case UNKNOWN:
			default:
				error.log("tried to set unsetable type " + toString(), INTERNAL_ERROR);
				break;
		}
	}
}

void* TypeData::cloneVoidPtr(void* val)
{
	void* ptr=createVoidPtr();
	setVoidPtr(ptr, val);
	return ptr;
	
	if (ptr)
	{
		switch (type)
		{
			case BOOL: return new bool(*((bool*)ptr));
			case INT: return new int(*((int*)ptr));
			case DUB: return new double(*((double*)ptr));
			case VOID: return nullptr;
			
			case TUPLE:
			case UNKNOWN:
			default:
				error.log("tried to clone unclonable type " + toString(), INTERNAL_ERROR);
				return nullptr;
		}
	}
	else
		return nullptr;
}

void* TypeData::castVoidPtr(void* val, Type typeOut)
{
	if (type==VOID || !val)
	{
		return nullptr;
	}
	else
	{
		void* ptr=typeOut->createVoidPtr();
		
		
		if (type==BOOL)
		{
			bool var=*((bool*)val);
			
			if (typeOut->matches(Bool))
				return new bool(ptr)=false;
			else if (typeOut->matches(Int))
				return new int(var?1:0);
			else if (typeOut->matches(Dub))
				return new double(var?1.0:0.0);
			else
				return nullptr;
		}
		else if (type==INT)
		{
			int var=*((int*)val);
			
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
			double var=*((double *)val);
			
			if (typeOut->matches(Bool))
				return new bool(var!=0);
			else if (typeOut->matches(Int))
				return new int((int)var);
			else if (typeOut->matches(Bool))
				return new double(var);
			else
				return nullptr;
		}
		else if (type==TUPLE)
		{
			ptr=nullptr;
			
			//vector<DataElem *> var=*((vector<DataElem *>)val->getData());
			
			if (type==Type::BOOL)
				return new BoolData(var!=0);
			else if (type==Type::INT)
				return new IntData((int)var);
			else if (type==Type::DUB)
				return new DubData(var);
			else
				return new VoidData();
			
			delete val;
		}
		else
		{
			error.log("unknown type " + toString() + " in CastElement::castToType", INTERNAL_ERROR);
			typeOut->deleteVoidPtr(ptr);
			ptr=nullptr;
		}
		
		return ptr;
	}
}*/

