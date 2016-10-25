#pragma once

#include <string>
using std::string;

#include <vector>
using std::vector;

#include <memory>
using std::shared_ptr;

class ActionTable;

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
		STRUCT,
	};
	
	static string toString(PrimitiveType in);
	string toString();
	
	//static Type getDominant(Type a, Type b);
	
	bool isCreatable();
	bool isVoid();
	
	size_t getSize();
	string getName() {return name;}
	
	PrimitiveType getType() {return type;}
	vector<shared_ptr<TypeBase>>& getTypes() {return types;}
	
	//bool operator==(const Type& other);
	//bool exactlyEquals(const Type& other);
	
	bool matches(shared_ptr<TypeBase> other);
	
	void* createVoidPtr();
	void* cloneVoidPtr(void* ptr);
	void* castVoidPtr(void* ptr, shared_ptr<TypeBase> typeOut);
	void setVoidPtr(void* ptr, void* in);
	void deleteVoidPtr(void* ptr);
	
	TypeBase(PrimitiveType typeIn, string nameIn) {type=typeIn; name=nameIn;}
	TypeBase(vector<shared_ptr<TypeBase>> typesIn, string nameIn);
private:
	
	friend ActionTable;
	
	
	string name;
	PrimitiveType type;
	vector<shared_ptr<TypeBase>> types;
};

typedef shared_ptr<TypeBase> Type;

const extern Type UnknownType;
const extern Type Void;
const extern Type Bool;
const extern Type Int;
const extern Type Dub;
