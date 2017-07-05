#include "../../h/Action.h"
#include "../../h/ErrorHandler.h"

class ListAction: public ActionData
{
public:
	
	ListAction(const vector<Action>& actionsIn, const vector<Action>& destroyersIn):
		ActionData
		(
			(actionsIn.size()>0?actionsIn.back()->getReturnType():Void),
			Void, Void
		)
	{
		actions=actionsIn;
		destroyers=destroyersIn;
		
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
		vector<string> data;
		
		for (auto i=actions.begin(); i!=actions.end(); ++i)
		{
			if (*i)
				data.push_back((*i)->getDescription());
			else
				data.push_back(str::putStringInTreeNodeBox("[null action]"));
		}
		
		return str::makeList(data);
		/*
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
		*/
	}
	
	void* execute(void* inLeft, void* inRight)
	{
		auto i=actions.begin();
		
		for (; i!=std::prev(actions.end()); ++i)
		{
			free((*i)->execute(nullptr, nullptr));
		}
		
		void* returnVal=(*i)->execute(nullptr, nullptr);
		
		for (auto j: destroyers)
		{
			free(j->execute(nullptr, nullptr));
		}
		
		return returnVal;
	}
	
	void addToProg(Action inLeft, Action inRight, CppProgram* prog)
	{
		addToProg(prog, getReturnType());
	}
	
	void addToProg(CppProgram* prog, Type returnType)
	{
		bool shouldReturn=(prog->getBlockLevel()==0 && prog->getIfReturnsVal()) && !prog->isMain();
		
		prog->pushBlock();
		
		for (auto i: actions)
		{
			if (shouldReturn && i==actions.back())
			{
				prog->declareVar("-out", returnType);
				prog->name("-out");
				prog->code(" = ");
				if (i->getReturnType()!=returnType)
				{
					cppTupleCastAction(i, returnType)->addToProg(prog);
				}
				else
				{
					i->addToProg(prog);
				}
			}
			else
			{
				i->addToProg(prog);
			}
			prog->endln();
		}
		
		for (auto i: destroyers)
		{
			i->addToProg(prog);
			prog->endln();
		}
		
		if (shouldReturn)
		{
			prog->code("return ");
			prog->name("-out");
			prog->endln();
		}
		
		prog->popBlock();
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
	vector<Action> destroyers;
};

void addListToProgWithCppCasting(ListAction* list, Type returnType, CppProgram* prog)
{
	list->addToProg(prog, returnType);
}

Action listAction(const vector<Action>& actionsIn, const vector<Action>& destroyersIn)
{
	if (actionsIn.size()==0 && destroyersIn.size()==0)
	{
		return voidAction;
	}
	else if (actionsIn.size()==1 && destroyersIn.size()==0)
	{
		return actionsIn[0];
	}
	else
	{
		return Action(new ListAction(actionsIn, destroyersIn));
	}
}

