#include "../../h/Action.h"
#include "../../h/ErrorHandler.h"

#include <vector>

using std::vector;

class MakeTupleAction: public ActionData
{
public:
	
	MakeTupleAction(const vector<Action>& sourceActionsIn):
		ActionData(
			[&]() -> Type
			{
				TupleTypeMaker tuple;
				
				for (auto i=sourceActionsIn.begin(); i!=sourceActionsIn.end(); ++i)
				{
					tuple.add((*i)->getReturnType());
				}
				
				return tuple.get();
			}(),
			
			Void, Void
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
		return "[tuple of type " + getReturnType()->getString() + "]";
		
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
	
	string getCSource(string inLeft, string inRight)
	{
		return "/* C source for MakeTupleAction not yet implemented */";
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
	
	vector<Action> sourceActions;
};

class GetElemFromTupleAction: public ActionData
{
public:
	
	GetElemFromTupleAction(Type typeInIn, Type typeOutIn, size_t offsetIn):
		ActionData(typeOutIn, Void, typeInIn)
	{
		typeOut=typeOutIn;
		offset=offsetIn;
	}

	string getDescription()
	{
		return "get element from tuple";
		
	}
	
	string getCSource(string inLeft, string inRight)
	{
		return "/* C source for GetElemFromTupleAction not yet implemented */";
	}
	
	void* execute(void* inLeft, void* inRight)
	{
		void* out=malloc(typeOut->getSize());
		memcpy(out, (char*)inRight+offset, typeOut->getSize());
		
		return out;
	}


	
private:
	Type typeOut;
	size_t offset;
};

Action makeTupleAction(const std::vector<Action>& sourceActionsIn)
{
	return Action(new MakeTupleAction(sourceActionsIn));
}

Action getElemFromTupleAction(Action source, string name)
{
	OffsetAndType a=source->getReturnType()->getSubType(name);
	
	if (a.type->isVoid())
	{
		error.log("could not find '"+name+"' in "+source->getReturnType()->getString(), SOURCE_ERROR);
		return voidAction;
	}
	
	Action action=Action(new GetElemFromTupleAction(source->getReturnType(), a.type, a.offset));
	
	Action out=branchAction(voidAction, action, source);
	
	return out;
}
