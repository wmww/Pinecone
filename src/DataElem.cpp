/*
#include "../h/DataElem.h"
#include "../h/ErrorHandler.h"

DataElem * DataElem::makeNewOfType(Type in)
{
	switch (in.getType())
	{
	case Type::BOOL:
		return new BoolData();
		
	case Type::INT:
		return new IntData();
		
	case Type::DUB:
		return new DubData();
		
	case Type::VOID:
		return new VoidData();
		
	case Type::STRUCT:
		{
			StructData * ptr=new StructData();
			
			for (auto i=in.getTypes().begin(); i!=in.getTypes().end(); ++i)
				ptr->addSubData(makeNewOfType(*i));
			
			return ptr;
		}
		
	default:
		error.log(string() + __FUNCTION__ + " sent invalid type of " + in.toString(), SOURCE_ERROR);
		return new VoidData();
	}
}

StructData::~StructData()
{
	for (auto i=subData.begin(); i!=subData.end(); ++i)
		delete *i;
}

void StructData::setData(void* in)
{
	vector<DataElem *> * ary=((vector<DataElem *> *)in);
	subData.clear();
	for (auto i=ary->begin(); i!=ary->end(); ++i)
	{
		subData.push_back((*i)->clone());
	}
}

DataElem * StructData::clone()
{
	StructData * ptr=new StructData();
	for (auto i=subData.begin(); i!=subData.end(); ++i)
		ptr->addSubData((*i)->clone());
	return ptr;
}

string StructData::toString()
{
	string out="(";
	
	auto i=subData.begin();
	
	while (i!=subData.end())
	{
		out+=(*i)->toString();
		++i;
		if (i!=subData.end())
			out+=", ";
	}
	
	out+=")";
	
	return out;
}



*/
