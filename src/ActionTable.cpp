#include "../h/ActionTable.h"
#include "../h/ErrorHandler.h"
#include "../h/OperatorElement.h"

void ActionTable::clear()
{
	actions.clear();
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
		parent->addActionsToList(matches, text);
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

void ActionTable::addActionsToList(list<ActionPtr>& in, OperatorType opType)
{
	in.insert(in.end(), operators[opType].begin(), operators[opType].end());
	
	if (parent)
		parent->addActionsToList(in, opType);
}

string ActionTable::toString()
{
	string out;
	
	out+="operators:\n";
	
	for (int i=0; i<OP_TYPE_OVERRIDEABLE_LAST; ++i)
	{
		for (auto j=operators[i].begin(); j!=operators[i].end(); ++j)
		{
			out+="\t";
			if (*j)
				out+=(*j)->toString();
			else
				out+="fucking null operator";
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
		if (leftIn.exactlyEquals((*i)->getInLeftType()) && rightIn.exactlyEquals((*i)->getInRightType()))
		{
			exactMatch=*i;
			break;
		}
		//else if ()
	}
	
	return exactMatch;
}

