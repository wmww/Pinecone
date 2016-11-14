#include "../../h/Action.h"
#include "../../h/ErrorHandler.h"

class BranchAction: public Action
{
public:
	BranchAction(ActionPtr leftInputIn, ActionPtr actionIn, ActionPtr rightInputIn)
			:Action(actionIn->getReturnType(), Void, Void, "BRANCH")
	{
		if (!actionIn)
			error.log(string() + "branch action created sent null action", INTERNAL_ERROR);
		
		if (!leftInputIn)
			error.log(string() + "branch action created sent null leftInput", INTERNAL_ERROR);
		
		if (!rightInputIn)
			error.log(string() + "branch action created sent null rightInput", INTERNAL_ERROR);
		
		action=actionIn;
		leftInput=leftInputIn;
		rightInput=rightInputIn;
		
		if (!leftInput->getInLeftType()->matches(Void) || !leftInput->getInRightType()->matches(Void))
		{
			error.log(leftInput->getDescription() + " put into branch even though its inputs are not void", INTERNAL_ERROR);
		}
		
		if (!rightInput->getInLeftType()->matches(Void) || !rightInput->getInRightType()->matches(Void))
		{
			error.log(rightInput->getDescription() + " put into branch even though its inputs are not void", INTERNAL_ERROR);
		}
		
		if (!leftInput->getReturnType()->matches(action->getInLeftType()))
		{
			error.log(leftInput->getDescription() + " return type is not the same as the left input of " + action->getDescription(), INTERNAL_ERROR);
		}
		
		if (!rightInput->getReturnType()->matches(action->getInRightType()))
		{
			error.log(rightInput->getDescription() + " return type is not the same as the right input of " + action->getDescription(), INTERNAL_ERROR);
		}
	}

	string getDescription()
	{
		if (leftInput && action && rightInput)
		{
			//return getReturnType()->toString() + " <- [" + leftInput->getDescription() + "].[" + action->getDescription() + "]:[" + rightInput->getDescription() + "]";
			return "(" + leftInput->getDescription() + " -> " + action->getDescription() + " <- " + rightInput->getDescription() + ")";
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
	
	string getCSource(string inLeft, string inRight)
	{
		return action->getCSource(leftInput->getCSource("", ""), rightInput->getCSource("", ""));
	}
	
private:
	ActionPtr action;
	ActionPtr leftInput;
	ActionPtr rightInput;
};

class RightBranchAction: public Action
{
public:
	RightBranchAction(ActionPtr actionIn, ActionPtr rightInputIn)
		:Action(actionIn->getReturnType(), Void, Void, "RIGHT_BRANCH")
	{
		if (!actionIn)
			error.log(string() + "branch action created sent null action", INTERNAL_ERROR);
			
		if (!rightInputIn)
			error.log(string() + "branch action created sent null rightInput", INTERNAL_ERROR);
		
		action=actionIn;
		rightInput=rightInputIn;
		
		if (!rightInput->getInLeftType()->matches(Void) || !rightInput->getInRightType()->matches(Void))
		{
			error.log(rightInput->getDescription() + " put into branch even though its inputs are not void", INTERNAL_ERROR);
		}
		
		if (!rightInput->getReturnType()->matches(action->getInRightType()))
		{
			error.log(rightInput->getDescription() + " return type is not the same as the right input of " + action->getDescription(), INTERNAL_ERROR);
		}
	}
	
	~RightBranchAction()
	{
		
	}

	string getDescription()
	{
		if (action && rightInput)
		{
			//return getReturnType()->toString() + " <- [" + leftInput->getDescription() + "].[" + action->getDescription() + "]:[" + rightInput->getDescription() + "]";
			return "(" + action->getDescription() + " <- " + rightInput->getDescription() + ")";
			//return getReturnType()->getName() + " <- " + leftInput->getDescription() + "." + action->getDescription() + ":" + rightInput->getDescription();
		}
		else
			return "[branch with null element]";
	}

	string getCSource(string inLeft, string inRight)
	{
		return action->getCSource("", rightInput->getCSource("", ""));
	}
	
	void* execute(void* inLeft, void* inRight)
	{
		void* rightData=rightInput->execute(nullptr, nullptr);
		void* outData=action->execute(nullptr, rightData);
		free(rightData);
		return outData;
	}
	
private:
	ActionPtr action;
	ActionPtr rightInput;
};

class LeftBranchAction: public Action
{
public:
	LeftBranchAction(ActionPtr leftInputIn, ActionPtr actionIn)
			:Action(actionIn->getReturnType(), Void, Void, "BRANCH")
	{
		if (!actionIn)
			error.log(string() + "branch action created sent null action", INTERNAL_ERROR);
		
		if (!leftInputIn)
			error.log(string() + "branch action created sent null leftInput", INTERNAL_ERROR);
		
		action=actionIn;
		leftInput=leftInputIn;
		
		if (!leftInput->getInLeftType()->matches(Void) || !leftInput->getInRightType()->matches(Void))
		{
			error.log(leftInput->getDescription() + " put into branch even though its inputs are not void", INTERNAL_ERROR);
		}
		
		if (!leftInput->getReturnType()->matches(action->getInLeftType()))
		{
			error.log(leftInput->getDescription() + " return type is not the same as the left input of " + action->getDescription(), INTERNAL_ERROR);
		}
	}

	string getDescription()
	{
		if (leftInput && action)
		{
			//return getReturnType()->toString() + " <- [" + leftInput->getDescription() + "].[" + action->getDescription() + "]:[" + rightInput->getDescription() + "]";
			return "(" + leftInput->getDescription() + " -> " + action->getDescription() + ")";
			//return getReturnType()->getName() + " <- " + leftInput->getDescription() + "." + action->getDescription() + ":" + rightInput->getDescription();
		}
		else
			return "[branch with null element]";
	}
	
	string getCSource(string inLeft, string inRight)
	{
		return action->getCSource(leftInput->getCSource("", ""), "");
	}

	void* execute(void* inLeft, void* inRight)
	{
		void* leftData=leftInput->execute(nullptr, nullptr);
		void* outData=action->execute(leftData, nullptr);
		free(leftData);
		return outData;
	}
	
private:
	ActionPtr leftInput;
	ActionPtr action;
};

ActionPtr branchAction(ActionPtr leftInputIn, ActionPtr actionIn, ActionPtr rightInputIn)
{
	if (leftInputIn->getReturnType()->isVoid())
	{
		if (rightInputIn->getReturnType()->isVoid())
		{
			return actionIn;
		}
		else
		{
			return ActionPtr(new RightBranchAction(actionIn, rightInputIn));
		}
	}
	else
	{
		if (rightInputIn->getReturnType()->isVoid())
		{
			return ActionPtr(new LeftBranchAction(leftInputIn, actionIn));
		}
		else
		{
			return ActionPtr(new BranchAction(leftInputIn, actionIn, rightInputIn));
		}
	}
}
