#pragma once

#include <string>
using std::string;

#include <unordered_map>
using std::unordered_map;

#include <memory>
using std::shared_ptr;

#include <cstring>
using std::memcpy;

class TypeBase;

typedef shared_ptr<TypeBase> Type;

class TypeBase
{
public:
	
	///steps to adding a new type (these may be old):
	///	1. Add to the enum (remember that order matters as operators will return the highest type involved
	///	2. add it to toString in Type.cpp
	///	3. add it to isCreatable in Type.cpp
	///	4. if it has a literal, add a class for that in LiteralElement.h
	/// 5. add the creation of said literal to LiteralElement::makeNew in LiteralElement.cpp
	///	6. you may have to add logic to initialProgramPopulation or getElementType to correctly capture the section of source containing your type literal
	///	7. add a class to DataElem.h
	/// 8. add it to makeNewOfType in DataElem.cpp
	
	enum PrimitiveType
	{
		UNKNOWN,
		VOID,
		DUB,
		INT,
		BOOL,
		TUPLE,
		METATYPE
	};
	
	static Type makeNew(Type typeIn, string nameIn);
	static Type makeNew(PrimitiveType typeIn, string nameIn);
	static Type makeNew(unordered_map<string, Type>, string nameIn);
	
	TypeBase(string nameIn) {name=nameIn;}
	
	static string toString(PrimitiveType in);
	virtual string getString()=0;
	
	virtual bool isCreatable() {return true};
	virtual bool isVoid() {return false;};
	
	size_t getSize();
	string getName() {return name;}
	
	virtual PrimitiveType getType() {return type;}
	
	bool matches(Type other);
	
private:
	
	string name;
	
	virtual bool matchesSameTypeType(Type other)=0;
};

const extern Type UnknownType;
const extern Type Void;
const extern Type Bool;
const extern Type Int;
const extern Type Dub;
