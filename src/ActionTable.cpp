#include "../h/ActionTable.h"
#include "../h/ErrorHandler.h"

void ActionTable::clear()
{
	for (auto i=actions.begin(); i!=actions.end(); ++i)
	{
		delete *i;
	}
	
	actions.clear();
}

Action * ActionTable::getBestAction(ElementData data, Type leftIn, Type rightIn)
{
	list<Action *> matches;
	
	addActionsToList(matches, data.text);
	
	return resolveOverload(matches, leftIn, rightIn);
}


void ActionTable::addActionsToList(list<Action *>& matches, string& text)
{
	for (auto i=actions.begin(); i!=actions.end(); ++i)
	{
		if ((*i)->getText()==text)
			matches.push_back(*i);
	}
	
	if (parent)
		parent->addActionsToList(matches, text);
}

Action * ActionTable::resolveOverload(list<Action *>& in, Type leftIn, Type rightIn)
{
	//Action * closeMatch=nullptr;
	Action * exactMatch=nullptr;
	
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

