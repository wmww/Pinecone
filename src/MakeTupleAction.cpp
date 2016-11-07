#include "../h/MakeTupleAction.h"
#include "../h/ErrorHandler.h"

#include <vector>

using std::vector;

class MakeTupleAction: public Action
{
public:
	
	MakeTupleAction(vector<ActionPtr>& sourceActionsIn):
		Action(
			Type(
				new TypeBase(
					[&]() -> vector<shared_ptr<TypeBase>>
					{
						vector<shared_ptr<TypeBase>> out;
						
						for (auto i=sourceActionsIn.begin(); i!=sourceActionsIn.end(); ++i)
						{
							out.push_back((*i)->getReturnType());
						}
						
						return out;
					}(), ""
				)
			),
			Void, Void, "TUPLE"
		)
	{
		if (sourceActionsIn.size()<=0)
		{
			error.log("MakeTupleAction created with empty list", INTERNAL_ERROR);
		}
		
		sourceActions=sourceActionsIn;
		
		for (auto i=sourceActions.begin(); i!=sourceActions.end(); ++i)
		{
			if (!(*i)->getInLeftType()->matches(Void) || !(*i)->getInRightType()->matches(Void))
			{
				error.log((*i)->getDescription() + " put into tuple creation even though its inputs are not void", INTERNAL_ERROR);
			}
			
			if ((*i)->getReturnType()->matches(Void))
			{
				error.log((*i)->getDescription() + " put into tuple creation even though its output is void", INTERNAL_ERROR);
			}
		}
	}

	string getDescription()
	{
		return "[tuple of type " + getReturnType()->toString() + "]";
		
		/*string out;
		out+="\n{";
		
		for (auto i=sourceActions.begin(); i!=sourceActions.end(); ++i)
		{
			out+="\n\t";
			
			string str;
			
			if (*i)
				str=(*i)->getDescription();
			else
				str="[null action]";
			
			for (unsigned j=0; j<str.size(); ++j)
			{
				out+=str[j];
				
				if (str[j]=='\n')
					out+='\t';
			}
			
			out+="\n";
		}
		
		out+="}\n";
		
		return out;*/
	}

	void* execute(void* inLeft, void* inRight)
	{
		void* out=malloc(getReturnType()->getSize());
		size_t offset=0;
		
		for (auto i=sourceActions.begin(); i!=sourceActions.end(); ++i)
		{
			void* val=(*i)->execute(nullptr, nullptr);
			memcpy((char*)out+offset, val, (*i)->getReturnType()->getSize());
			free(val);
			offset+=(*i)->getReturnType()->getSize();
		}
		
		return out;
	}


	
private:
	
	vector<ActionPtr> sourceActions;
};

ActionPtr makeTupleAction(std::vector<ActionPtr>& sourceActionsIn)
{
	return ActionPtr(new MakeTupleAction(sourceActionsIn));
}
