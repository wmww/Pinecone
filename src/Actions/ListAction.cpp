#include "../../h/Action.h"
#include "../../h/ErrorHandler.h"

class ListAction: public ActionData
{
public:
	
	ListAction(const vector<Action>& actionsIn):
		ActionData
		(
			(actionsIn.size()>0?actionsIn.back()->getReturnType():Void),
			Void, Void
		)
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
	
	~ListAction() 
	{
		
	}

	string getDescription()
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
	
	void* execute(void* inLeft, void* inRight)
	{
		auto i=actions.begin();
		
		for (; i!=std::prev(actions.end()); ++i)
		{
			free((*i)->execute(nullptr, nullptr));
		}
		
		return (*i)->execute(nullptr, nullptr);
	}
	
	void addCppCodeToProg(CppProgram& prog)
	{
		prog.pushBlock();
		
		for (auto i: actions)
		{
			i->addCppCodeToProg(prog);
		}
		
		prog.popBlock();
	}
	
	/*
	string getCSource(string inLeft, string inRight)
	{
		string out;
		
		for (auto i: actions)
		{
			out+=i->getCSource()+";\n";
		}
		
		return out;
	}
	*/
	
	
	
private:
	vector<Action> actions;
};

Action listAction(const vector<Action>& actionsIn)
{
	if (actionsIn.size()==0)
	{
		return voidAction;
	}
	else if (actionsIn.size()==1)
	{
		return actionsIn[0];
	}
	else
	{
		return Action(new ListAction(actionsIn));
	}
}
