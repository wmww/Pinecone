#pragma once

#include <string>
using std::string;

#include <vector>
using std::vector;

#include <memory>
using std::shared_ptr;
using std::unique_ptr;

#include <cstring>
using std::memcpy;

class TypeBase;

typedef shared_ptr<TypeBase> Type;

const extern Type Unknown;
const extern Type Whatev;
const extern Type Void;
const extern Type Bool;
const extern Type Byte;
const extern Type Int;
const extern Type Dub;
extern Type String;

class CppProgram;
class ActionData;

struct NamedType
{
	string name;
	Type type;
};

struct OffsetAndType
{
	size_t offset;
	Type type;
};

class TypeBase: public std::enable_shared_from_this<TypeBase>
{
public:

	virtual ~TypeBase() = default;
	
	///steps to adding a new type (these may be old):
	///	1. Add to the enum
	///	2. add it to toString in Type.cpp
	///	3. add it to isCreatable in Type.cpp
	///	4. if it has a literal, add a class for that in LiteralElement.h
	/// 5. add the creation of said literal to LiteralElement::makeNew in LiteralElement.cpp
	///	6. you may have to add logic to initialProgramPopulation or getElementType to correctly capture the section of source containing your type literal
	///	7. add a class to DataElem.h
	/// 8. add it to makeNewOfType in DataElem.cpp
	
	enum PrimitiveType
	{
		UNKNOWN ,
		VOID ,
		BYTE,
		DUB,
		INT,
		PTR,
		BOOL,
		TUPLE,
		WHATEV,
		METATYPE,
	};
	
	/* type compact forms
	Void: v
	Unknown: u
	Bool: b
	Byte: y
	Int: i
	Dub: d
	Ptr: Pp_..._pP
	Tuple: Tt_[size of name]_name_type..._tT
	Whatev: W
	Meta: Mm_..._mM
	*/
	
	static Type makeNewVoid();
	static Type makeNewPrimitive(PrimitiveType typeIn);
	static Type makeNewWhatev();
	//static Type makeNew(unordered_map<string, Type>, string nameIn);
	
	//TypeBase(string nameIn) {name=nameIn;}
	
	Type getMeta();
	Type getPtr();
	virtual Type actuallyIs(Type target); // returns a type with with the whatevs resolved with the target type
	
	static string getString(PrimitiveType in);
	virtual string getString()=0;
	string getName() {return nameHint.empty()?getCompactString():nameHint;}
	virtual string getCompactString()=0; // returns a string that is unique, only shared with other types that match this one
	virtual string getCppLiteral(void * data, CppProgram * prog)=0;
	
	virtual bool isCreatable() {return true;};
	virtual bool isVoid() {return false;};
	virtual bool isWhatev() {return false;};
	
	virtual size_t getSize()=0;
	//string getName() {return name;}
	
	virtual PrimitiveType getType()=0;
	
	bool matches(Type other);
	
	virtual Type getSubType() {return Void;}
	virtual OffsetAndType getSubType(string name) {return {0, nullptr};}
	
	virtual vector<NamedType>* getAllSubTypes(); // will thow an error if this is not a tuple type
	
	// void setNameHint(const string& in) {if (nameHint.empty()) nameHint=in;} // only sets name hint on any type once, after that it fails quietly
	// string getNameHint
	
	string nameHint=""; // not reliable, will often be ""; should only be used as a hint, not depended on
	
protected:
	
	virtual bool matchesSameTypeType(Type other)=0;
	
	Type ptrToMe=nullptr;
};

Type makeTuple(const vector<NamedType>& in, bool isAnonymous);

//	since types are immutable, this class is an easy way to construct a tuple type
class TupleTypeMaker
{
public:
	TupleTypeMaker();
	void add(string name, Type type);

	void add(Type type);
	
	Type get(bool isAnonymous);
	
private:
	
	string getUniqueName();
	
	std::unique_ptr<vector<NamedType>> subTypes;
};
