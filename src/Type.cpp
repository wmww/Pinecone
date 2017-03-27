#include "../h/Type.h"
#include "../h/ErrorHandler.h"
#include "../h/CppProgram.h"
#include "../h/msclStringFuncs.h"

#include <sstream>

using std::unique_ptr;
using std::get;

class VoidType: public TypeBase
{
public:
	virtual string getString()
	{
		return "VOID";
	}
	
	string getCompactString()
	{
		return "v";
	}
	
	string getCppLiteral(void * data, CppProgram * prog)
	{
		throw PineconeError("tried to get the literal value of 'void;", INTERNAL_ERROR);
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
	
	string getCompactString()
	{
		return "u";
	}
	
	string getCppLiteral(void * data, CppProgram * prog)
	{
		return "/* can not instantiate unknown type */";
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
	
	string getCompactString()
	{
		switch (primType)
		{
			case BOOL: return "b";
			case BYTE: return "y";
			case INT: return "i";
			case DUB: return "d";
			
			default:
				throw PineconeError("tried to make " + getString() + " compact", INTERNAL_ERROR);
		}
	}
	
	string getString()
	{
		return TypeBase::getString(primType);
	}
	
	string getCppLiteral(void * data, CppProgram * prog)
	{
		string val;
		
		switch (primType)
		{
			case BOOL: val=(*(bool*)data)?"true":"false"; break;
			case BYTE: val=to_string(*(unsigned char*)data); break;
			case INT: val=to_string(*(int*)data); break;
			case DUB: val=doubleToString(*(double*)data); break;
			/*{
				std::ostringstream ss;
				ss << *(double*)data;
				val=ss.str();
			}
			break;*/
			
			default:
				throw PineconeError("tried to convert " + getString() + " to C++ code", INTERNAL_ERROR);
		}
		
		return val;
	}
	
	size_t getSize()
	{
		switch (primType)
		{
			case BOOL: return sizeof(bool);
			case BYTE: return sizeof(unsigned char);
			case INT: return sizeof(int);
			case DUB: return sizeof(double);
			
			default:
				throw PineconeError("tried to get size of " + getString(), INTERNAL_ERROR);
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
	
	TupleType(unique_ptr<vector<NamedType>> in, bool isAnonymousIn)
	{
		if (in==nullptr)
			error.log(FUNC+" sent null input, compiler will likely shit itself in the near future", INTERNAL_ERROR);
		
		subTypes=std::move(in);
		isAnonymous=isAnonymousIn;
	}
	
	~TupleType()
	{
		
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
		
		out+="} (tuple)";
		
		return out;
	}
	
	string getCompactString()
	{
		string out;
		
		out+="Tt_";
		
		for (int i=0; i<int(subTypes->size()); i++)
		{
			out+=(*subTypes)[i].name.size()+"_"+(*subTypes)[i].name+"_"+(*subTypes)[i].type->getCompactString()+"_";
		}
		
		out+="tT";
		
		return out;
	}
	
	string getCppLiteral(void * data, CppProgram * prog)
	{
		string out;
		out+=prog->getTypeCode(shared_from_this());
		
		out+="(";
		
		for (int i=0; i<int(subTypes->size()); i++)
		{
			if (i)
				out+=", ";
			
			out+=(*subTypes)[i].type->getCppLiteral((char*)data+getSubType((*subTypes)[i].name).offset, prog);
		}
		
		out+=")";
		
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
	
	OffsetAndType getSubType(string name)
	{
		size_t offset=0;
		
		for (auto i: *subTypes)
		{
			if (i.name==name)
				return {offset, i.type};
			else
				offset+=i.type->getSize();
		}
		
		return {0, nullptr};
	}
	
	vector<NamedType>* getAllSubTypes()
	{
		return &(*subTypes);
	}
	
protected:
	
	bool matchesSameTypeType(Type other)
	{
		auto o=(TupleType*)(&(*other));
		
		if (!isAnonymous && !o->isAnonymous)
			return false;
		
		if (subTypes->size()!=o->subTypes->size())
			return false;
		
		for (int i=0; i<int(subTypes->size()); i++)
		{
			//if ((*subTypes)[i].name!=(*o->subTypes)[i].name)
			//	return false;
			
			//if ((*subTypes)[i].type!=(*o->subTypes)[i].type)
			//	return false;
			
			if (!(*subTypes)[i].type->matches((*o->subTypes)[i].type))
				return false;
		}
		
		return true;
	}
	
private:
	
	unique_ptr<vector<NamedType>> subTypes;
	bool isAnonymous=true;
};

class PtrType: public TypeBase
{
public:
	
	PtrType(Type in)
	{
		type=in;
	}
	
	string getString()
	{
		return "-> "+type->getString()+" (pointer)";
	}
	
	string getCompactString()
	{
		return "Pp_"+type->getCompactString()+"_pP";
	}
	
	string getCppLiteral(void * data, CppProgram * prog)
	{
		string name=to_string((long long)data);
		prog->declareGlobal(name, type, type->getCppLiteral(*(void**)data, prog));
		string out="&"+prog->getGlobalNames()->getCpp(name);
		return out;
	}
	
	size_t getSize()
	{
		return sizeof(void*);
	}
	
	PrimitiveType getType()
	{
		return PTR;
	}
	
	Type getSubType()
	{
		return type;
	}
	
protected:
	
	Type type;
	
	bool matchesSameTypeType(Type other)
	{
		return ((PtrType*)(&(*other)))->type->matches(type);
	}
};

class MetaType: public TypeBase
{
public:
	
	MetaType(Type in)
	{
		type=in;
	}
	
	string getString()
	{
		return "{"+type->getString()+"} (meta type)";
	}
	
	string getCompactString()
	{
		return "Mm_"+type->getCompactString()+"_mM";
	}
	
	string getCppLiteral(void * data, CppProgram * prog)
	{
		return "/* can't add meta type to C++ code */";
	}
	
	size_t getSize()
	{
		return 0;
	}
	
	bool isCreatable()
	{
		return false;
	}
	
	PrimitiveType getType()
	{
		return METATYPE;
	}
	
	Type getSubType()
	{
		return type;
	}
	
protected:
	
	Type type;
	
	bool matchesSameTypeType(Type other)
	{
		return ((MetaType*)(&(*other)))->type==type;
	}
};

class WhatevType: public TypeBase
{
public:
	
	WhatevType() {}
	
	string getString()
	{
		return "(whatev type)";
	}
	
	string getCompactString()
	{
		return "W";
	}
	
	string getCppLiteral(void * data, CppProgram * prog)
	{
		throw PineconeError("getCppLiteral called on whatev type, wich should not have happened", INTERNAL_ERROR);
	}
	
	size_t getSize()
	{
		throw PineconeError("getSize called on whatev type, wich should not have happened", INTERNAL_ERROR);
	}
	
	PrimitiveType getType()
	{
		return GENERIC;
	}
	
	bool isWhatev()
	{
		return true;
	}
	
	Type getSubType()
	{
		throw PineconeError("getSubType called on whatev type, wich should not have happened", INTERNAL_ERROR);
	}
	
protected:
	
	Type type;
	
	bool matchesSameTypeType(Type other)
	{
		return true;
	}
};

Type TypeBase::makeNewVoid()
{
	return Type(new VoidType);
}

Type TypeBase::makeNewPrimitive(PrimitiveType typeIn)
{
	return Type(new PrimType(typeIn));
}

Type TypeBase::makeNewWhatev()
{
	return Type(new WhatevType());
}

vector<NamedType>* TypeBase::getAllSubTypes()
{
	throw PineconeError("getAllSubTypes called on type that was not a tuple", INTERNAL_ERROR);
}

const Type Unknown(new UnknownType);
const Type Void = TypeBase::makeNewVoid();
const Type Whatev = TypeBase::makeNewWhatev();
const Type Bool = TypeBase::makeNewPrimitive(TypeBase::BOOL);
const Type Byte = TypeBase::makeNewPrimitive(TypeBase::BYTE);
const Type Int = TypeBase::makeNewPrimitive(TypeBase::INT);
const Type Dub = TypeBase::makeNewPrimitive(TypeBase::DUB);
Type String = nullptr;

Type TypeBase::getMetaType()
{
	return Type(new MetaType(shared_from_this()));
}

Type TypeBase::getPtr()
{
	if (!ptrToMe)
		ptrToMe=Type(new PtrType(shared_from_this()));
	
	return ptrToMe;
}

string TypeBase::getString(PrimitiveType in)
{
	switch (in)
	{
		case UNKNOWN: return "UNKNOWN_TYPE";
		case VOID: return "VOID";
		case BOOL: return "BOOL";
		case BYTE: return "BYTE";
		case INT: return "INT";
		case DUB: return "DUB";
		case TUPLE: return "TUPLE";
		case METATYPE: return "METATYPE";
		default: return "ERROR_GETTING_TYPE";
	}
}

bool TypeBase::matches(Type other)
{
	if (other==shared_from_this())
		return true;
	
	auto otherType=other->getType();
	
	if (otherType!=getType())
	{
		return false;
	}
	else if (!matchesSameTypeType(other))
	{
		return false;
	}
	else
	{
		return true;
	}
}

Type makeTuple(const vector<NamedType>& in, bool isAnonymous)
{
	auto ptr=unique_ptr<vector<NamedType>>(new vector<NamedType>(in));
	return Type(new TupleType(move(ptr), isAnonymous));
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

Type TupleTypeMaker::get(bool isAnonymous)
{
	if (subTypes)
		return Type(new TupleType(std::move(subTypes), isAnonymous));
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


