#pragma once

#include <string>
using std::string;

#include <vector>
using std::vector;

#include <memory>
using std::shared_ptr;

#include <cstring>
using std::memcpy;

class ActionTable;

class TypeData
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
	
	TypeData(shared_ptr<TypeData> typeIn, string nameIn);
	TypeData(PrimitiveType typeIn, string nameIn);
	TypeData(PrimitiveType shouldBeMetatype, shared_ptr<TypeData> typeIn, string nameIn);
	TypeData(vector<shared_ptr<TypeData>> typesIn, string nameIn);
	
	static string toString(PrimitiveType in);
	string toString();
	
	//static Type getDominant(Type a, Type b);
	
	bool isCreatable();
	bool isVoid();
	
	size_t getSize();
	string getName() {return name;}
	
	PrimitiveType getType() {return type;}
	vector<shared_ptr<TypeData>>& getTypes() {return types;}
	
	//bool operator==(const Type& other);
	//bool exactlyEquals(const Type& other);
	
	bool matches(shared_ptr<TypeData> other);
	
	//void* createVoidPtr();
	//void* cloneVoidPtr(void* ptr);
	//void* castVoidPtr(void* ptr, shared_ptr<TypeData> typeOut);
	//void setVoidPtr(void* ptr, void* in);
	//void deleteVoidPtr(void* ptr);
	
private:
	
	string name;
	PrimitiveType type;
	vector<shared_ptr<TypeData>> types;
};

typedef shared_ptr<TypeData> Type;

Type newMetatype(Type typeIn);

const extern Type UnknownType;
const extern Type Void;
const extern Type Bool;
const extern Type Int;
const extern Type Dub;
