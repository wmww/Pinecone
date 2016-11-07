#include "../h/ActionTable.h"
#include "../h/ErrorHandler.h"
#include "../h/OperatorElement.h"
#include "../h/Action.h"

ActionTable::ActionTable(shared_ptr<ActionTable> parentIn)
{
	parent=parentIn;
	stackFrame=parent->getStackFrame();
}

ActionTable::ActionTable(StackFrame * stackFrameIn)
{
	parent=nullptr;
	stackFrame=stackFrameIn;
}

void ActionTable::clear()
{
	actions.clear();
	converters.clear();
	types.clear();
	
	operators.clear();
}

void ActionTable::addAction(ActionPtr in)
{
	Type type=getType(in->getText());
	
	if (type)
	{
		if (in->getReturnType()!=type)
		{
			error.log("cannot create a function called " + in->getText() + " that does not return the type of the same name", SOURCE_ERROR);
			return;
		}
		else
		{
			actions.push_back(in);
			converters.push_back(in);
		}
	}
	else
	{
		actions.push_back(in);
	}
}

void ActionTable::addAction(ActionPtr in, Operator op)
{
	operators.push_back(in);
}

void ActionTable::addActionsToList(vector<ActionPtr>& matches, string& text)
{
	for (auto i=actions.begin(); i!=actions.end(); ++i)
	{
		if ((*i)->getText()==text)
			matches.push_back(*i);
	}
	
	if (parent)
	{
		parent->addActionsToList(matches, text);
	}
}

ActionPtr ActionTable::addConverter(ActionPtr action, vector<Type>& types)
{
	for (auto i=types.begin(); i!=types.end(); i++)
	{
		if (action->getReturnType()==(*i))
		{
			return action;
		}
	}
	
	vector<ActionPtr> typeConverters;
	
	getAllConvertersForType(typeConverters, action->getReturnType());
	
	for (auto i=types.begin(); i!=types.end(); i++)
	{
		for (auto j=typeConverters.begin(); j!=typeConverters.end(); j++)
		{
			if ((*j)->getReturnType()==(*i))
			{
				return branchAction(voidAction, *j, action);
			}
		}
	}
	
	return voidAction;
}

ActionPtr ActionTable::makeBranchAction(ElementData data, ActionPtr left, ActionPtr right)
{
	vector<ActionPtr> matches;
	
	addActionsToList(matches, data.text);
	
	if (matches.empty())
	{
		return voidAction;
	}
	else
	{
		ActionPtr out=makeBranchAction(matches, left, right);
		
		if (out==voidAction)
		{
			error.log("no overload found for " + data.text, SOURCE_ERROR);
		}
		
		return out;
	}
}

ActionPtr ActionTable::makeBranchAction(ElementData data, Operator op, ActionPtr left, ActionPtr right)
{
	vector<ActionPtr> matches;
	
	addActionsToList(matches, op);
	
	ActionPtr out=makeBranchAction(matches, left, right);
	
	return out;
}

ActionPtr ActionTable::makeBranchAction(vector<ActionPtr>& matches, ActionPtr left, ActionPtr right)
{
	Type leftType=left->getReturnType();
	Type rightType=right->getReturnType();
	
	ActionPtr action=nullptr;
	
	//look for exact match
	
	for (auto i=matches.begin(); i!=matches.end(); ++i)
	{
		if (leftType->matches((*i)->getInLeftType()) && rightType->matches((*i)->getInRightType()))
		{
			action=*i;
			break;
		}
	}
	
	//look for match that requires casting
	
	if (!action)
	{
		vector<ActionPtr> leftConverters;
		vector<ActionPtr> rightConverters;
		
		getAllConvertersForType(leftConverters, leftType);
		getAllConvertersForType(rightConverters, rightType);
		
		vector<vector<ActionPtr>> actionsBranches;
		
		for (auto i=matches.begin(); i!=matches.end(); ++i)
		{
			ActionPtr rightMatch=nullptr;
			ActionPtr leftMatch=nullptr;
			bool failed=false;
			
			if (leftType!=(*i)->getInLeftType())
			{
				for (auto j=leftConverters.begin(); j!=leftConverters.end(); ++j)
				{
					if ((*j)->getReturnType()==(*i)->getInLeftType())
					{
						leftMatch=*j;
						break;
					}
				}
				
				if (!leftMatch)
					failed=true;
			}
			
			if (!failed && rightType!=(*i)->getInRightType())
			{
				for (auto j=rightConverters.begin(); j!=rightConverters.end(); ++j)
				{
					if ((*j)->getReturnType()==(*i)->getInRightType())
					{
						rightMatch=*j;
						break;
					}
				}
				
				if (!rightMatch)
					failed=true;
			}
			
			if (!failed)
			{
				vector<ActionPtr> ptrs(3);
				
				if (leftMatch)
					ptrs[0]=branchAction(voidAction, leftMatch, left);
				else
					ptrs[0]=left;
				
				ptrs[1]=*i;
				
				if (rightMatch)
					ptrs[2]=branchAction(voidAction, rightMatch, right);
				else
					ptrs[2]=right;
				
				actionsBranches.push_back(ptrs);
			}
		}
		
		if (actionsBranches.empty())
		{
			return voidAction;
		}
		else if (actionsBranches.size()==1)
		{
			return branchAction((*actionsBranches.begin())[0], (*actionsBranches.begin())[1], (*actionsBranches.begin())[2]);
		}
		{
			return voidAction;
			
			/*auto best=actionsBranches.begin();
			
			for (auto i=std::next(actionsBranches.begin()); i!=actionsBranches.end(); i++)
			{
				TypeBase::DOM_LEVEL leftDm=(*i)[0]->getReturnType().getDominance((*best)[0]->getReturnType())
				TypeBase::DOM_LEVEL rightDm=(*i)[2]->getReturnType().getDominance((*best)[2]->getReturnType())
				
				if ()
			}
			
			return ActionPtr(new BranchAction((*best)[0], (*best)[1], (*best)[2])); */
		}
	}
	else
	{
		return branchAction(left, action, right);
	}
}

void ActionTable::getAllConvertersForType(vector<ActionPtr>& convertersOut, Type type)
{
	if (type->getName().empty() && type->getType()==TypeBase::TUPLE)
	{
		error.log("recursive tuple converter search not yet implemented in ActionTable::getAllConvertersForType", INTERNAL_ERROR);
	}
	else
	{
		for (auto i=converters.begin(); i!=converters.end(); ++i)
		{
			if ((*i)->getInLeftType()==Void && (*i)->getInRightType()==type)
			{
				convertersOut.push_back(*i);
			}
		}
	}
}

Type ActionTable::getType(string name)
{
	for (auto i=types.begin(); i!=types.end(); ++i)
	{
		if ((*i)->getName()==name)
			return *i;
	}
	
	if (parent)
	{
		return parent->getType(name);
	}
	else
	{
		return Type(nullptr);
	}
}

void ActionTable::addActionsToList(vector<ActionPtr>& in, Operator op)
{
	for (auto i=operators.begin(); i!=operators.end(); ++i)
	{
		if ((*i)->getText()==op->getText())
			in.push_back(*i);
	}
	
	if (parent)
		parent->addActionsToList(in, op);
}

void ActionTable::addType(Type typeIn)
{
	Type preExisting=getType(typeIn->getName());
	
	if (preExisting)
	{
		error.log("type " + typeIn->getName() + " can not be created as it is already type " + preExisting->toString(), SOURCE_ERROR);
		return;
	}
	
	types.push_back(Type(typeIn));
}

void ActionTable::addType(TypeBase::PrimitiveType typeIn, string nameIn)
{
	Type preExisting=getType(nameIn);
	
	if (preExisting)
	{
		error.log("type " + nameIn + " can not be created as it is already type " + preExisting->toString(), SOURCE_ERROR);
		return;
	}
	
	types.push_back(Type(new TypeBase(typeIn, nameIn)));
}

void ActionTable::addType(vector<Type> typesIn, string nameIn)
{
	Type preExisting=getType(nameIn);
	
	if (preExisting)
	{
		error.log("type " + nameIn + " can not be created as it is already type " + preExisting->toString(), SOURCE_ERROR);
		return;
	}
	
	types.push_back(Type(new TypeBase(typesIn, nameIn)));
}

string ActionTable::toString()
{
	string out;
	
	out+="types:\n";
	for (auto i=types.begin(); i!=types.end(); ++i)
	{
		out+="\t";
		if (*i)
			out+=(*i)->getName() + " (" + (*i)->toString() + ")";
		else
			out+="null type";
		out+="\n";
	}
	
	out+="\noperators:\n";
	
	for (auto j=operators.begin(); j!=operators.end(); ++j)
	{
		out+="\t";
		if (*j)
			out+=(*j)->toString();
		else
			out+="null operator";
		out+="\n";
	}
	
	out+="\nconverters:\n";
	
	for (auto i=converters.begin(); i!=converters.end(); ++i)
	{
		out+="\t";
		if (*i)
			out+=(*i)->toString();
		else
			out+="null action";
		out+="\n";
	}
	
	out+="\nnormal functions:\n";
	
	for (auto i=actions.begin(); i!=actions.end(); ++i)
	{
		out+="\t";
		if (*i)
			out+=(*i)->toString();
		else
			out+="null action";
		out+="\n";
	}
	
	return out;
}

