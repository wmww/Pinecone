#include "../../h/Action.h"

#include "../../h/ErrorHandler.h"

#include "../../h/msclStringFuncs.h"

ActionData::ActionData(Type returnTypeIn, Type inLeftTypeIn, Type inRightTypeIn)
{
	returnType=returnTypeIn;
	inLeftType=inLeftTypeIn;
	inRightType=inRightTypeIn;
	
	if (!returnType || !inLeftType || !inRightType)
	{
		throw PineconeError("ActionData created with null type", INTERNAL_ERROR);
	}
}

string ActionData::toString()
{
	return description;
	
	//return returnType->getName() + " <- " + inLeftType->getName() + " " + text + " " + inRightType->getName();
}

string ActionData::getTypesString()
{
	return returnType->getString()+" <- "+inLeftType->getString()+"."+inRightType->getString();
}

//LambdaAction::LambdaAction(Type returnTypeIn, function<void*(void*,void*)> lambdaIn, Type inLeftTypeIn, Type inRightTypeIn, string textIn)
	
//void* LambdaAction::execute(void* inLeft, void* inRight)

class VoidAction: public ActionData
{
public:
	VoidAction(): ActionData(Void, Void, Void)
	{
		setDescription("Void Action");
	}
	
	void* execute(void* inLeft, void* inRight)
	{
		return nullptr;
	}
	
	void addToProg(Action inLeft, Action inRight, CppProgram* prog)
	{
		if (prog->getExprLevel()>0)
			prog->comment("void");
	}
	
	string getDescription()
	{
		return str::putStringInTreeNodeBox("void");
	}
};

class LambdaAction: public ActionData
{
public:
	LambdaAction(Type inLeftTypeIn, Type inRightTypeIn, Type returnTypeIn,
		function<void*(void*,void*)> lambdaIn,
		function<void(Action inLeft, Action inRight, CppProgram* prog)> cppWriterIn,
		string textIn)
			: ActionData(returnTypeIn, inLeftTypeIn, inRightTypeIn)
	{
		if (cppWriterIn==nullptr)
		{
			cppWriter=[=](Action inLeft, Action inRight, CppProgram* prog)
			{
				prog->comment("lambda action '"+textIn+"' has not yet been implemented for C++");
			};
		}
		else
		{
			cppWriter=cppWriterIn;
		}
		
		if (lambdaIn==nullptr)
		{
			lambdaIn=[=](void* inLeft, void* inRight)->void*
			{
				throw PineconeError("action '"+textIn+"' has not yet been implemented for the interpreter", RUNTIME_ERROR);
			};
		}
		else
		{
			lambda=lambdaIn;
		}
		
		setDescription(textIn);
	}
	
	void* execute(void* inLeft, void* inRight)
	{
		return lambda(inLeft, inRight);
	}
	
	void addToProg(Action inLeft, Action inRight, CppProgram* prog)
	{
		cppWriter(inLeft, inRight, prog);
	}
	
	string getDescription()
	{
		return str::putStringInTreeNodeBox(description);
	}
	
private:
	function<void*(void*,void*)> lambda;
	function<void(Action inLeft, Action inRight, CppProgram* prog)> cppWriter;
	string cppCode;
};

Action lambdaAction(Type inLeftTypeIn, Type inRightTypeIn, Type returnTypeIn,
	function<void*(void*,void*)> lambdaIn,
	function<void(Action inLeft, Action inRight, CppProgram* prog)> cppWriter,
	string textIn)
{
	return Action(new LambdaAction(inLeftTypeIn, inRightTypeIn, returnTypeIn, lambdaIn, cppWriter, textIn));
}

Action createNewVoidAction()
{
	return Action(new VoidAction());
}
