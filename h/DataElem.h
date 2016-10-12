#pragma once

#include "Type.h"
#include <vector>

using std::vector;

class DataElem
{
public:
	
	static DataElem * makeNewOfType(Type in);
	
	virtual ~DataElem() {}
	
	virtual Type getType()=0;
	
	virtual void * getData()=0;
	
	virtual void setData(void * in)=0;
	
	virtual DataElem * clone()=0;
};

class StructData: public DataElem
{
public:
	
	StructData() {}
	StructData(vector<DataElem *>& in) {subData=in;}
	
	~StructData();
	
	Type getType() {return Type(Type::STRUCT);}
	
	void * getData() {return &subData;}
	
	void setData(void * in);
	
	DataElem * clone();
	
	void addSubData(DataElem * in) {subData.push_back(in);}
	
private:
	
	vector<DataElem *> subData;
};

class VoidData: public DataElem
{
public:
	
	Type getType() {return Type(Type::VOID);}
	
	void * getData() {return nullptr;}
	
	void setData(void * in) {}
	
	DataElem * clone() {return new VoidData();}
};

class BoolData: public DataElem
{
public:
	
	BoolData() {data=0;}
	BoolData(bool in) {data=in;}
	
	Type getType() {return Type(Type::BOOL);}
	
	void * getData() {return &data;}
	
	void setData(void * in) {data=*((bool*)in);}
	
	DataElem * clone() {return new BoolData(data);}
	
private:
	
	bool data;
};

class IntData: public DataElem
{
public:
	
	IntData() {data=0;}
	IntData(int in) {data=in;}
	
	Type getType() {return Type(Type::INT);}
	
	void * getData() {return &data;}
	
	void setData(void * in) {data=*((int*)in);}
	
	DataElem * clone() {return new IntData(data);}
	
private:
	
	int data;
};

class DubData: public DataElem
{
public:
	
	DubData() {data=0;}
	DubData(double in) {data=in;}
	
	Type getType() {return Type(Type::DUB);}
	
	void * getData() {return &data;}
	
	void setData(void * in) {data=*((double*)in);}
	
	DataElem * clone() {return new DubData(data);}
	
private:
	
	double data;
};

