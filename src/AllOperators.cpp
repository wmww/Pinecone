

#include "../h/AllOperators.h"
#include "../h/ErrorHandler.h"

//shared_ptr<AllOperators> ops(nullptr);
AllOperators* ops=nullptr;

Operator opCreate(string textIn, int leftPrecedenceIn, int rightPrecedenceIn, bool overloadableIn);

void AllOperators::init()
{
	//ops=shared_ptr<AllOperators>(new AllOperators());
	ops=new AllOperators();
}

AllOperators::AllOperators()
{	
	#undef DECLARE_OP
	
	//#define DECLARE_OP(name, text, left, right, overload) putOpInMap(name);
	
	#define DECLARE_OP(name, text, prece, input, overload) putOpInMap(name);
	
	ALL_OPS;
}

void AllOperators::putOpInMap(Operator op)
{
	opsMap[op->getText()]=op;
}

void AllOperators::get(string text, vector<Operator>& out)
{
	int start=0;
	int end=text.size();
	
	while (start<int(text.size()))
	{
		while (true)
		{
			if (end<=start)
			{
				error.log("unknown operator '" + text + "'", SOURCE_ERROR);
			}
			
			auto i=opsMap.find(text.substr(start, end-start));
			
			if (i==opsMap.end())
			{
				end--;
			}
			else
			{
				out.push_back(i->second);
				start=end;
				end=text.size();
				break;
			}
		}
	}
}

bool AllOperators::isOpenBrac(Operator op)
{
	return op==openPeren || op==openSqBrac || op==openCrBrac;
}

bool AllOperators::isCloseBrac(Operator op)
{
	return op==closePeren || op==closeSqBrac || op==closeCrBrac;
}

/*
// this is the only way to make an operator, and should only be called when setting up all the global operators at the top of Operator.cpp
Operator opCreate(string textIn, int leftPrecedenceIn, int rightPrecedenceIn, bool overloadableIn)
{
	Operator ptr(new OperatorData(textIn, leftPrecedenceIn, rightPrecedenceIn, overloadableIn));
	
	
	for (auto i=OperatorData::precedenceLevels.begin();; i++)
	{
		if (i==OperatorData::precedenceLevels.end() || *i>leftPrecedenceIn)
		{
			OperatorData::precedenceLevels.insert(i, leftPrecedenceIn);
			break;
		}
		else if (*i==leftPrecedenceIn)
			break;
	}
	
	for (auto i=OperatorData::precedenceLevels.begin();; i++)
	{
		if (i==OperatorData::precedenceLevels.end() || *i>rightPrecedenceIn)
		{
			OperatorData::precedenceLevels.insert(i, rightPrecedenceIn);
			break;
		}
		else if (*i==rightPrecedenceIn)
			break;
	}
	
	//operators.push_back(ptr);
	return ptr;
}
*/


