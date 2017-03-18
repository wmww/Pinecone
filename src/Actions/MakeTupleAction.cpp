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
	
	void addToProg(Action inLeft, Action inRight, CppProgram* prog)
	{
		prog->code(prog->getTypeCode(getReturnType()));
		prog->pushExpr();
		bool start=true;
		for (auto i: sourceActions)
		{
			if (!start) prog->code(", ");
			start=false;
			
			i->addToProg(prog);
		}
		prog->popExpr();
	}
	
	
private:
	
	vector<Action> sourceActions;
};

class GetElemFromTupleAction: public ActionData
{
public:
	
	GetElemFromTupleAction(Type typeInIn, string nameIn):
		ActionData(typeInIn->getSubType(nameIn).type, typeInIn, Void)
	{
		typeIn=typeInIn;
		typeOut=typeInIn->getSubType(nameIn).type;
		// if no type was found, the ActionData constructor would have already thrown an error
		name=nameIn;
	}
	
	string getDescription()
	{
		return "get element from tuple";
	}
	
	void* execute(void* inLeft, void* inRight)
	{
		void* out=malloc(typeOut->getSize());
		memcpy(out, (char*)inLeft+typeIn->getSubType(name).offset, typeOut->getSize());
		
		return out;
	}
	
	void addToProg(Action inLeft, Action inRight, CppProgram* prog)
	{
		inRight->addToProg(prog);
		prog->code("."+name);
	}
	
private:
	Type typeIn;
	Type typeOut;
	string name;
};

Action makeTupleAction(const std::vector<Action>& sourceActionsIn)
{
	return Action(new MakeTupleAction(sourceActionsIn));
}

Action getElemFromTupleAction(Type source, string name)
{
	if (!source->getSubType(name).type)
		throw PineconeError("could not find '"+name+"' in "+source->getString(), SOURCE_ERROR);
	
	Action out=Action(new GetElemFromTupleAction(source, name));
	
	return out;
}

