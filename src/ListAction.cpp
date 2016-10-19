#include "../h/ListAction.h"
#include "../h/ErrorHandler.h"

ListAction::ListAction(list<ActionPtr>& actionsIn): Action(Type(Type::VOID), Type(Type::VOID), Type(Type::VOID), "LIST")
{
	actions=actionsIn;
	
	for (auto i=actions.begin(); i!=actions.end(); ++i)
	{
		if ((*i)->getInLeftType().getType()!=Type::VOID || (*i)->getInRightType().getType()!=Type::VOID)
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
		
		string str=(*i)->getDescription();
		
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
	for (auto i=actions.begin(); i!=actions.end(); ++i)
	{
		(*i)->getReturnType().deleteVoidPtr((*i)->execute(nullptr, nullptr));
	}
	
	return nullptr;
}

