#pragma once

#include <string>
using std::string;

#include <vector>
using std::vector;

class Type
{
public:
	
	///steps to adding a new type:
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
	
	static Type getDominant(Type a, Type b);
	
	bool isCreatable();
	bool isVoid();
	
	size_t getSize();
	
	Type() {type=UNKNOWN;}
	Type(PrimitiveType typeIn) {type=typeIn;}
	Type(vector<Type>& typesIn);
	
	PrimitiveType getType() {return type;}
	vector<Type>& getTypes() {return types;}
	
	bool operator==(const Type& other);
	bool exactlyEquals(const Type& other);
	
	void* createVoidPtr();
	void* cloneVoidPtr(void* ptr);
	void* castVoidPtr(void* ptr, Type typeOut);
	void setVoidPtr(void* ptr, void* in);
	void deleteVoidPtr(void* ptr);
	
private:
	
	PrimitiveType type;
	vector<Type> types;
};

