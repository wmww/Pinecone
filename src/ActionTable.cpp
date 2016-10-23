#include "../h/ActionTable.h"
#include "../h/ErrorHandler.h"
#include "../h/OperatorElement.h"
#include "../h/BranchAction.h"
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
	
	//types.push_back(Void);
	//types.push_back(Bool);
	//types.push_back(Int);
	//types.push_back(Dub);
}

void ActionTable::clear()
{
	actions.clear();
	converters.clear();
	types.clear();
	
	for (int i=0; i<OP_TYPE_OVERRIDEABLE_LAST; i++)
		operators[i].clear();
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

void ActionTable::addAction(ActionPtr in, OperatorType opType)
{
	if (opType<0 || opType>OP_TYPE_OVERRIDEABLE_LAST)
	{
		error.log(string() + __FUNCTION__ + " sent invalid opType", INTERNAL_ERROR);
		return;
	}
	
	operators[opType].push_back(in);
}

ActionPtr ActionTable::getBestAction(ElementData data, Type leftIn, Type rightIn)
{
	vector<ActionPtr> matches;
	
	addActionsToList(matches, data.text);
	
	return resolveOverload(matches, leftIn, rightIn);
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

ActionPtr ActionTable::getBestAction(OperatorType opType, Type leftIn, Type rightIn)
{
	if (opType<0 || opType>OP_TYPE_OVERRIDEABLE_LAST)
	{
		error.log(string() + __FUNCTION__ + " sent invalid opType", INTERNAL_ERROR);
		return nullptr;
	}
	
	vector<ActionPtr> matches;
	
	addActionsToList(matches, opType);
	
	return resolveOverload(matches, leftIn, rightIn);
}

ActionPtr ActionTable::makeBranchAction(ElementData data, OperatorType opType, ActionPtr left, ActionPtr right)
{
	if (opType<0 || opType>OP_TYPE_OVERRIDEABLE_LAST)
	{
		error.log(string() + __FUNCTION__ + " sent invalid opType '" + OperatorElement::toString(opType) + "'", INTERNAL_ERROR);
		return nullptr;
	}
	
	Type leftType=left->getReturnType();
	Type rightType=right->getReturnType();
	
	vector<ActionPtr> matches;
	
	addActionsToList(matches, opType);
	
	ActionPtr action=nullptr;
	
	for (auto i=matches.begin(); i!=matches.end(); ++i)
	{
		if (leftType==(*i)->getInLeftType() && rightType==(*i)->getInRightType())
		{
			action=*i;
			break;
		}
	}
	
	if (!action)
	{
		vector<ActionPtr> leftConverters;
		vector<ActionPtr> rightConverters;
		
		for (auto i=converters.begin(); i!=converters.end(); ++i)
		{
			if ((*i)->getInLeftType()==Void && (*i)->getInRightType()==leftType)
			{
				leftConverters.push_back(*i);
			}
			
			if ((*i)->getInLeftType()==Void && (*i)->getInRightType()==rightType)
			{
				rightConverters.push_back(*i);
			}
		}
		
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
				if (rightMatch)
					right=ActionPtr(new RightBranchAction(rightMatch, right));
				
				if (leftMatch)
					left=ActionPtr(new RightBranchAction(leftMatch, left));
				
				return ActionPtr(new BranchAction(left, *i, right));
			}
		}
		
		return voidAction;
	}
	else
	{
		return ActionPtr(new BranchAction(left, action, right));
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

void ActionTable::addActionsToList(vector<ActionPtr>& in, OperatorType opType)
{
	in.insert(in.end(), operators[opType].begin(), operators[opType].end());
	
	if (parent)
		parent->addActionsToList(in, opType);
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
	
	for (int i=0; i<OP_TYPE_OVERRIDEABLE_LAST; ++i)
	{
		for (auto j=operators[i].begin(); j!=operators[i].end(); ++j)
		{
			out+="\t";
			if (*j)
				out+=(*j)->toString();
			else
				out+="null operator";
			out+="\n";
		}
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
	
	out+="\nregular functions:\n";
	
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

ActionPtr ActionTable::resolveOverload(vector<ActionPtr>& in, Type leftIn, Type rightIn)
{
	//ActionPtr closeMatch=nullptr;
	ActionPtr exactMatch=nullptr;
	
	for (auto i=in.begin(); i!=in.end(); ++i)
	{
		if (leftIn==(*i)->getInLeftType() && rightIn==(*i)->getInRightType())
		{
			exactMatch=*i;
			break;
		}
		//else if ()
	}
	
	return exactMatch;
}

