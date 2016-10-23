#include "../h/ActionTable.h"
#include "../h/ErrorHandler.h"
#include "../h/OperatorElement.h"

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
}

void ActionTable::addAction(ActionPtr in)
{
	Type ptr=getType(in->getText());
	
	if (!ptr || in->getReturnType()==ptr)
		actions.push_back(in);
	else
	{
		error.log("cannot create a function called " + in->getText() + " that does not return the type of the same name", SOURCE_ERROR);
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
	list<ActionPtr> matches;
	
	addActionsToList(matches, data.text);
	
	return resolveOverload(matches, leftIn, rightIn);
}

void ActionTable::addActionsToList(list<ActionPtr>& matches, string& text)
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
	
	list<ActionPtr> matches;
	
	addActionsToList(matches, opType);
	
	return resolveOverload(matches, leftIn, rightIn);
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

void ActionTable::addActionsToList(list<ActionPtr>& in, OperatorType opType)
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
	
	out+="\nother:\n";
	
	for (auto i=actions.begin(); i!=actions.end(); ++i)
	{
		out+="\t";
		if (*i)
			out+=(*i)->toString();
		else
			out+="null operator";
		out+="\n";
	}
	
	return out;
}

ActionPtr ActionTable::resolveOverload(list<ActionPtr>& in, Type leftIn, Type rightIn)
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

