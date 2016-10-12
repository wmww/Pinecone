#include "../h/CastElement.h"
#include "../h/ErrorHandler.h"

void CastElement::printToString(string& in, int depth)
{
	printIndentationToString(in, depth);
	in+="cast to ";
	in+=type.toString();
	in+="\n";
	
	if (input)
		input->printToString(in, depth+1);
}

DataElem * CastElement::execute()
{
	if (!input)
	{
		return new VoidData();
	}
	
	DataElem * ptr=input->execute();
	
	DataElem * data=castToType(ptr, type);
	
	delete ptr;
	
	return data;
}

DataElem * CastElement::castToType(DataElem * ptr, Type type)
{
	
	if (ptr->getType()==Type::BOOL)
	{
		bool var=*((bool *)ptr->getData());
		
		if (type==Type::BOOL)
			return new BoolData(var);
		else if (type==Type::INT)
			return new IntData(var?1:0);
		else if (type==Type::DUB)
			return new DubData(var?1.0:0.0);
		else
			return new VoidData();
	}
	else if (ptr->getType()==Type::INT)
	{
		int var=*((int *)ptr->getData());
		
		if (type==Type::BOOL)
			return new BoolData(var!=0);
		else if (type==Type::INT)
			return new IntData(var);
		else if (type==Type::DUB)
			return new DubData((double)var);
		else
			return new VoidData();
	}
	else if (ptr->getType()==Type::DUB)
	{
		double var=*((double *)ptr->getData());
		
		if (type==Type::BOOL)
			return new BoolData(var!=0);
		else if (type==Type::INT)
			return new IntData((int)var);
		else if (type==Type::DUB)
			return new DubData(var);
		else
			return new VoidData();
	}
	else if (ptr->getType()==Type::STRUCT)
	{
		return new VoidData();
		
		/*//vector<DataElem *> var=*((vector<DataElem *>)ptr->getData());
		
		if (type==Type::BOOL)
			return new BoolData(var!=0);
		else if (type==Type::INT)
			return new IntData((int)var);
		else if (type==Type::DUB)
			return new DubData(var);
		else
			return new VoidData();
		
		delete ptr;*/
	}
	else if (ptr->getType()==Type::VOID)
	{
		return new VoidData();
	}
	else
	{
		error.log("unknown type " + ptr->getType().toString() + " in CastElement::castToType", INTERNAL_ERROR);
		return new VoidData();
	}
}

