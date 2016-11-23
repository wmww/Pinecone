
#include "../../h/Action.h"
#include "../../h/ErrorHandler.h"
#include "../../h/StackFrame.h"

#include <cstring> //for memcpy
using std::memcpy;

class FunctionAction: public ActionData
{
public:
	
	FunctionAction(Action actionIn, Type inLeftTypeIn, Type inRightTypeIn, int dataSizeIn):
		ActionData(actionIn->getReturnType(), inLeftTypeIn, inRightTypeIn)
	{
		dataSize=dataSizeIn;
		action=actionIn;
		
		if (action->getInLeftType()!=Void || action->getInRightType()!=Void)
		{
			error.log(action->getDescription() + " put into function even though its inputs are not void", INTERNAL_ERROR);
		}
	}

	string getDescription()
	{
		return "func: " + action->getDescription();
	}
	
	string getCSource(string inLeft, string inRight)
	{
		string out;
		out+="/* function C source not yet implemented, but here is the action: */";
		out+="\n*/\n";
		out+=action->getCSource();
		out+="\n*/";
		return out;
		
		/*string name=getGlobalCName();
		
		string globalCFunc;
		globalCFunc+=action->getReturnType()->getCSource();
		globalCFunc+=" ";
		globalCFunc+=name;
		globalCFunc+="("
		if (!inLeft.empty())
			globalCFunc+=inLeft+","
		globalCFunc+=
		globalCFunc+=
		globalCFunc+=
		action->getCSource()
		addGlobalCSource();*/
	}
	
	void* execute(void* inLeft, void* inRight)
	{
		unsigned char * oldStackPtr=stackPtr;
		stackPtr=new unsigned char[dataSize];
		
		if (inLeft)
			memcpy(stackPtr, inLeft, getInLeftType()->getSize());
		
		if (inRight)
			memcpy(stackPtr+getInLeftType()->getSize(), inRight, getInRightType()->getSize());
		
		void* out=action->execute(nullptr, nullptr);
		
		delete[] stackPtr;
		stackPtr=oldStackPtr;
		
		return out;
	}
	
private:
	
	int dataSize;
	Action action;
};

Action functionAction(Action actionIn, Type inLeftTypeIn, Type inRightTypeIn, int dataSizeIn)
{
	return Action(new FunctionAction(actionIn, inLeftTypeIn, inRightTypeIn, dataSizeIn));
}
