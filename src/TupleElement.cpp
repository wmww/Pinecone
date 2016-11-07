#include "../h/TupleElement.h"
#include "../h/MakeTupleAction.h"
#include "../h/ErrorHandler.h"

void TupleElement::appendElement(ElementPtr in)
{
	elems.push_back(in);
}

ActionPtr TupleElement::resolveActions(ActionTablePtr table)
{
	if (elems.empty())
	{
		error.log("TupleElement used with no elements", data, INTERNAL_ERROR);
		return voidAction;
	}
	else if (elems.size()==1)
	{
		error.log("TupleElement used with only 1 element", data, INTERNAL_ERROR);
		return (*elems.begin())->resolveActions(table);
	}
	else
	{
		vector<ActionPtr> actions;
		
		for (auto i=elems.begin(); i!=elems.end(); ++i)
		{
			actions.push_back((*i)->resolveActions(table));
		}
		
		return makeTupleAction(actions);
	}
}

void TupleElement::printToString(string& in, int depth)
{
	printIndentationToString(in, depth);
	in+="(\n";
	
	for (auto i=elems.begin(); i!=elems.end();)
	{
		(*i)->printToString(in, depth+2);
		
		++i;
		
		printIndentationToString(in, depth);
		
		if (i!=elems.end())
			in+=",\n";
	}
	
	in+=")\n";
}

list<ElementPtr> elems;
