#include "../h/ListAction.h"
#include "../h/ErrorHandler.h"

ListAction::ListAction(list<ActionPtr>& actionsIn): Action((actionsIn.size()>0?actionsIn.back()->getReturnType():UnknownType), Void, Void, "LIST")
{
	if (actionsIn.size()<=0)
	{
		error.log("ListAction created with empty list", INTERNAL_ERROR);
	}
	
	actions=actionsIn;
	
	for (auto i=actions.begin(); i!=actions.end(); ++i)
	{
		if (!(*i)->getInLeftType()->matches(Void) || !(*i)->getInRightType()->matches(Void))
		{
			error.log((*i)->getDescription() + " put into action list even though its inputs are not void", INTERNAL_ERROR);
		}
	}
}

string ListAction::getDescription()
{
	string out;
	out+="\n{";
	
	for (auto i=actions.begin(); i!=actions.end(); ++i)
	{
		out+="\n\t";
		
		string str;
		
		if (*i)
			str=(*i)->getDescription();
		else
			str="[null action]";
		
		error.log(str, JSYK);
		
		for (unsigned j=0; j<str.size(); ++j)
		{
			out+=str[j];
			
			if (str[j]=='\n')
				out+='\t';
		}
		
		out+="\n";
	}
	
	out+="}\n";
	
	return out;
}
	
void* ListAction::execute(void* inLeft, void* inRight)
{
	auto i=actions.begin();
	
	for (; i!=std::prev(actions.end()); ++i)
	{
		(*i)->getReturnType()->deleteVoidPtr((*i)->execute(nullptr, nullptr));
	}
	
	return (*i)->execute(nullptr, nullptr);
}

