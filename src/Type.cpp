#include "../h/Type.h"
#include "../h/ErrorHandler.h"

using std::unique_ptr;
using std::get;

class VoidType: public TypeBase
{
public:
	virtual string getString()
	{
		return "VOID";
	}
	
	bool isCreatable()
	{
		return false;
	}
	
	bool isVoid()
	{
		return true;
	}
	
	size_t getSize()
	{
		return 0;
	}
	
	PrimitiveType getType()
	{
		return VOID;
	};
	
protected:
	
	bool matchesSameTypeType(Type other)
	{
		return true;
	}
};

class UnknownType: public TypeBase
{
public:
	virtual string getString()
	{
		return "UNKNOWN";
	}
	
	bool isCreatable()
	{
		return false;
	}
	
	bool isVoid()
	{
		return false;
	}
	
	size_t getSize()
	{
		return 0;
	}
	
	PrimitiveType getType()
	{
		return UNKNOWN;
	};
	
protected:
	
	bool matchesSameTypeType(Type other)
	{
		return false;
	}
};

class PrimType: public TypeBase
{
public:
	
	PrimType(PrimitiveType in)
	{
		primType=in;
	}
	
	string getString()
	{
		return TypeBase::getString(primType);
	}
	
	size_t getSize()
	{
		switch (primType)
		{
			case BOOL: return sizeof(bool);
			case INT: return sizeof(int);
			case DUB: return sizeof(double);
			
			default:
				error.log("tried to get size of " + getString(), INTERNAL_ERROR);
				return 0;
		}
	}
	
	PrimitiveType getType()
	{
		return primType;
	}
	
protected:
	
	PrimitiveType primType;
	
	bool matchesSameTypeType(Type other)
	{
		return other->getType()==primType;
	}
};

class TupleType: public TypeBase
{
public:
	
	TupleType(unique_ptr<vector<NamedType>> in)
	{
		if (in==nullptr)
			error.log(FUNC+" sent null input, compiler will likely shit itself in the near future", INTERNAL_ERROR);
		
		subTypes=std::move(in);
	}
	
	string getString()
	{
		string out;
		
		out+="{";
		
		for (int i=0; i<int(subTypes->size()); i++)
		{
			if (i)
				out+=", ";
			
			out+=(*subTypes)[i].name+": "+(*subTypes)[i].type->getString();
		}
		
		out+="}";
		
		return out;
	}
	
	size_t getSize()
	{
		size_t sum=0;
		
		for (auto i: *subTypes)
		{
			sum+=i.type->getSize();
		}
		
		return sum;
	}
	
	PrimitiveType getType()
	{
		return TUPLE;
	}
	
protected:
	
	bool matchesSameTypeType(Type other)
	{
		auto o=(TupleType*)(&(*other));
		
		if (subTypes->size()!=o->subTypes->size())
			return false;
		
		for (int i=0; i<int(subTypes->size()); i++)
		{
			if ((*subTypes)[i].name!=(*o->subTypes)[i].name)
				return false;
			
			if ((*subTypes)[i].type!=(*o->subTypes)[i].type)
				return false;
		}
		
		return true;
	}
	
private:
	
	unique_ptr<vector<NamedType>> subTypes;
};

Type TypeBase::makeNewVoid()
{
	return Type(new VoidType);
}

Type TypeBase::makeNewPrimitive(PrimitiveType typeIn)
{
	return Type(new PrimType(typeIn));
}

Type TypeBase::makeNewMeta(Type typeIn)
{
	//return Type(new PrimType(typeIn));
	
	error.log(FUNC+" is not yet implemented", INTERNAL_ERROR);
	
	return Void;
}

const Type Unknown(new UnknownType);
const Type Void = TypeBase::makeNewVoid();
const Type Bool = TypeBase::makeNewPrimitive(TypeBase::BOOL);
const Type Int = TypeBase::makeNewPrimitive(TypeBase::INT);
const Type Dub = TypeBase::makeNewPrimitive(TypeBase::DUB);

Type TypeBase::getMetaType()
{
	return Unknown;
}

string TypeBase::getString(PrimitiveType in)
{
	switch (in)
	{
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

TupleTypeMaker::TupleTypeMaker()
{
	subTypes=unique_ptr<vector<NamedType>>(new vector<NamedType>);
}

void TupleTypeMaker::add(string name, Type type)
{
	if (subTypes)
		subTypes->push_back(NamedType{name, type});
	else
		error.log(FUNC+"called after type has been created", INTERNAL_ERROR);
}

void TupleTypeMaker::add(Type type)
{
	string name=getUniqueName();
	
	if (!name.empty())
	{
		add(name, type);
	}
	else
	{
		error.log("finding unique name in tuple type failed", INTERNAL_ERROR);
	}
}

Type TupleTypeMaker::get()
{
	if (subTypes)
		return Type(new TupleType(std::move(subTypes)));
	else
		error.log(FUNC+"called after type has been created", INTERNAL_ERROR);
	
	return Void;
}

string TupleTypeMaker::getUniqueName()
{
	for (char c='a'; c<='z'; ++c)
	{
		string str;
		str+=c;
		bool valid=true;
		
		for (auto i: *subTypes)
		{
			if (i.name==str)
			{
				valid=false;
				break;
			}
		}
		
		if (valid)
			return str;
	}
	
	error.log("you've gotta be fuckin kidding", SOURCE_ERROR);
	return "";
}

/*
class TupleType: public TypeBase
{
public:
	
	virtual string getString()
	{
		string out;
		
		for (i: typesOrder)
		{
			out+=
		}
	}
	
	virtual size_t getSize()
	{
		size_t sum=0;
		
		for (i: typesOrder)
		{
			sum+=i->getSize();
		}
		
		return sum;
	}
	
	virtual PrimitiveType getType()
	{
		return TUPLE;
	}
	
private:
	
	unordered_map<string, Type> typesMap;
	vector<Type> typesOrder;
	
	virtual bool matchesSameTypeType(Type other)
	{
		return other->getType()==primType;
	}
};
*/

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


