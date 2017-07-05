#include "../../h/Action.h"
#include "../../h/ErrorHandler.h"
#include "../../h/CppProgram.h"
#include "../../h/utils/stringDrawing.h"

class BranchAction: public ActionData
{
public:
	BranchAction(Action leftInputIn, Action actionIn, Action rightInputIn)
			:ActionData(actionIn->getReturnType(), Void, Void)
	{
		if (!actionIn)
			throw PineconeError(string() + "branch action created sent null action", INTERNAL_ERROR);
		
		if (!leftInputIn)
			throw PineconeError(string() + "branch action created sent null leftInput", INTERNAL_ERROR);
		
		if (!rightInputIn)
			throw PineconeError(string() + "branch action created sent null rightInput", INTERNAL_ERROR);
		
		action=actionIn;
		leftInput=leftInputIn;
		rightInput=rightInputIn;
		
		if (!leftInput->getInLeftType()->matches(Void) || !leftInput->getInRightType()->matches(Void))
		{
			throw PineconeError(leftInput->getDescription() + " put into branch even though its inputs are not void", INTERNAL_ERROR);
		}
		
		if (!rightInput->getInLeftType()->matches(Void) || !rightInput->getInRightType()->matches(Void))
		{
			throw PineconeError(rightInput->getDescription() + " put into branch even though its inputs are not void", INTERNAL_ERROR);
		}
		
		if (!leftInput->getReturnType()->matches(action->getInLeftType()))
		{
			throw PineconeError(leftInput->getDescription() + " return type is not the same as the left input of " + action->getDescription(), INTERNAL_ERROR);
		}
		
		if (!rightInput->getReturnType()->matches(action->getInRightType()))
		{
			throw PineconeError(rightInput->getDescription() + " return type is not the same as the right input of " + action->getDescription(), INTERNAL_ERROR);
		}
	}

	string getDescription()
	{
		if (leftInput && action && rightInput)
		{
			return str::makeRootUpBinaryTree(action->getDescription(), leftInput->getReturnType()->getName(), rightInput->getReturnType()->getName(), leftInput->getDescription(), rightInput->getDescription());
			//return getReturnType()->toString() + " <- [" + leftInput->getDescription() + "].[" + action->getDescription() + "]:[" + rightInput->getDescription() + "]";
			//return "(" + leftInput->getDescription() + " -> " + action->getDescription() + " <- " + rightInput->getDescription() + ")";
			//return getReturnType()->getName() + " <- " + leftInput->getDescription() + "." + action->getDescription() + ":" + rightInput->getDescription();
		}
		else
			return "[branch with null element]";
	}

	void* execute(void* inLeft, void* inRight)
	{
		void* leftData=leftInput->execute(nullptr, nullptr);
		void* rightData=rightInput->execute(nullptr, nullptr);
		void* outData=action->execute(leftData, rightData);
		free(leftData);
		free(rightData);
		return outData;
	}
	
	void addToProg(Action inLeft, Action inRight, CppProgram* prog)
	{
		Action leftInTmp = leftInput;
		Action rightInTmp = rightInput;
		
		if (leftInTmp->getReturnType()!=action->getInLeftType())
			leftInTmp=cppTupleCastAction(leftInTmp, action->getInLeftType());
		
		if (rightInTmp->getReturnType()!=action->getInRightType())
			rightInTmp=cppTupleCastAction(rightInTmp, action->getInRightType());
		
		action->addToProg(leftInTmp, rightInTmp, prog);
	}
	
private:
	Action action;
	Action leftInput;
	Action rightInput;
};

class RightBranchAction: public ActionData
{
public:
	RightBranchAction(Action actionIn, Action rightInputIn)
		:ActionData(actionIn->getReturnType(), Void, Void)
	{
		if (!actionIn)
			throw PineconeError(string() + "branch action created sent null action", INTERNAL_ERROR);
			
		if (!rightInputIn)
			throw PineconeError(string() + "branch action created sent null rightInput", INTERNAL_ERROR);
		
		action=actionIn;
		rightInput=rightInputIn;
		
		if (!rightInput->getInLeftType()->matches(Void) || !rightInput->getInRightType()->matches(Void))
		{
			throw PineconeError(rightInput->getDescription() + " put into branch even though its inputs are not void", INTERNAL_ERROR);
		}
		
		if (!rightInput->getReturnType()->matches(action->getInRightType()))
		{
			throw PineconeError(rightInput->getDescription() + " return type is not the same as the right input of " + action->getDescription(), INTERNAL_ERROR);
		}
	}
	
	~RightBranchAction()
	{
		
	}

	string getDescription()
	{
		if (action && rightInput)
		{
			return str::makeRootUpBinaryTree(action->getDescription(), "", rightInput->getReturnType()->getName(), "", rightInput->getDescription());
			//return getReturnType()->toString() + " <- [" + leftInput->getDescription() + "].[" + action->getDescription() + "]:[" + rightInput->getDescription() + "]";
			//return "(" + action->getDescription() + " <- " + rightInput->getDescription() + ")";
			//return getReturnType()->getName() + " <- " + leftInput->getDescription() + "." + action->getDescription() + ":" + rightInput->getDescription();
		}
		else
			return "[branch with null element]";
	}
	
	void* execute(void* inLeft, void* inRight)
	{
		void* rightData=rightInput->execute(nullptr, nullptr);
		void* outData=action->execute(nullptr, rightData);
		free(rightData);
		return outData;
	}
	
	void addToProg(Action inLeft, Action inRight, CppProgram* prog)
	{
		Action rightInTmp = rightInput;
		
		if (rightInTmp->getReturnType()!=action->getInRightType())
			rightInTmp=cppTupleCastAction(rightInTmp, action->getInRightType());
		
		action->addToProg(voidAction, rightInTmp, prog);
	}
	
private:
	Action action=nullptr;
	Action rightInput=nullptr;
};

class LeftBranchAction: public ActionData
{
public:
	LeftBranchAction(Action leftInputIn, Action actionIn)
			:ActionData(actionIn->getReturnType(), Void, Void)
	{
		if (!actionIn)
			throw PineconeError(string() + "branch action created sent null action", INTERNAL_ERROR);
		
		if (!leftInputIn)
			throw PineconeError(string() + "branch action created sent null leftInput", INTERNAL_ERROR);
		
		action=actionIn;
		leftInput=leftInputIn;
		
		if (!leftInput->getInLeftType()->matches(Void) || !leftInput->getInRightType()->matches(Void))
		{
			throw PineconeError(leftInput->getDescription() + " put into branch even though its inputs are not void", INTERNAL_ERROR);
		}
		
		if (!leftInput->getReturnType()->matches(action->getInLeftType()))
		{
			throw PineconeError(leftInput->getDescription() + " return type is not the same as the left input of " + action->getDescription(), INTERNAL_ERROR);
		}
	}

	string getDescription()
	{
		if (leftInput && action)
		{
			return str::makeRootUpBinaryTree(action->getDescription(), leftInput->getReturnType()->getName(), "", leftInput->getDescription(), "");
			//return getReturnType()->toString() + " <- [" + leftInput->getDescription() + "].[" + action->getDescription() + "]:[" + rightInput->getDescription() + "]";
			//return "(" + leftInput->getDescription() + " -> " + action->getDescription() + ")";
			//return getReturnType()->getName() + " <- " + leftInput->getDescription() + "." + action->getDescription() + ":" + rightInput->getDescription();
		}
		else
			return "[branch with null element]";
	}

	void* execute(void* inLeft, void* inRight)
	{
		void* leftData=leftInput->execute(nullptr, nullptr);
		void* outData=action->execute(leftData, nullptr);
		free(leftData);
		return outData;
	}
	
	void addToProg(Action inLeft, Action inRight, CppProgram* prog)
	{
		Action leftInTmp = leftInput;
		
		if (leftInTmp->getReturnType()!=action->getInLeftType())
			leftInTmp=cppTupleCastAction(leftInTmp, action->getInLeftType());
		
		action->addToProg(leftInTmp, voidAction, prog);
	}
	
private:
	Action leftInput;
	Action action;
};

Action branchAction(Action leftInputIn, Action actionIn, Action rightInputIn)
{
	//return Action(new BranchAction(leftInputIn, actionIn, rightInputIn));
	
	
	if (leftInputIn->getReturnType()->isVoid())
	{
		if (rightInputIn->getReturnType()->isVoid())
		{
			return actionIn;
		}
		else
		{
			return Action(new RightBranchAction(actionIn, rightInputIn));
		}
	}
	else
	{
		if (rightInputIn->getReturnType()->isVoid())
		{
			return Action(new LeftBranchAction(leftInputIn, actionIn));
		}
		else
		{
			return Action(new BranchAction(leftInputIn, actionIn, rightInputIn));
		}
	}
}
