#include "../../h/Action.h"
#include "../../h/ErrorHandler.h"

#include <vector>
#include <typeinfo>

using std::vector;

class GetElemFromTupleAction;
class CppTupleCastAction;

class ListAction;
void addListToProgWithCppCasting(ListAction* list, Type returnType, CppProgram* prog);

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
				
				return tuple.get(true);
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
		return str::putStringInTreeNodeBox("make tuple of type "+getReturnType()->getName());
		
		//return "[tuple of type " + getReturnType()->getString() + "]";
		
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
		if (sourceActions.size()==1)
		{
			sourceActions[0]->addToProg(prog);
			return;
		}
		
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
	
	friend GetElemFromTupleAction;
	friend CppTupleCastAction;
};

class CppTupleCastAction: public ActionData
{
public:
	
	CppTupleCastAction(Action actionIn, Type returnType):
		ActionData(returnType, Void, Void)
	{
		if ((actionIn->getReturnType()->getType()!=TypeBase::TUPLE && getReturnType()->getType()!=TypeBase::TUPLE) || !actionIn->getReturnType()->matches(getReturnType()))
		{
			throw PineconeError("CppCastAction was only designed to cast matching tuples, which is not how it is being used", INTERNAL_ERROR);
		}
		
		action=actionIn;
	}
	
	string getDescription()
	{
		return "C++ cast";
	}
	
	void* execute(void* inLeft, void* inRight)
	{
		throw PineconeError("CppCastAction was executed in the interpreter, which shouldn't happen", INTERNAL_ERROR);
	}
	
	void addToProg(Action inLeft, Action inRight, CppProgram* prog)
	{
		if (getReturnType()->getAllSubTypes()->size()==1)
		{
			action->addToProg(prog);
		}
		else if (typeid(*action)==typeid(MakeTupleAction))
		{
			MakeTupleAction * realAction=(MakeTupleAction*)&*action;
			
			prog->code(prog->getTypeCode(getReturnType()));
			prog->pushExpr();
				for (auto i: realAction->sourceActions)
				{
					i->addToProg(prog);
					if (i!=realAction->sourceActions.back())
						prog->code(", ");
				}
			prog->popExpr();
		}
		else if (typeid(*action)==typeid(*listAction({voidAction, voidAction}, {})))
		{
			addListToProgWithCppCasting((ListAction*)&*action, getReturnType(), prog);
		}
		else if (getReturnType()->getType()!=TypeBase::TUPLE)
		{
			action->addToProg(prog);
			prog->code(".");
			prog->code(action->getReturnType()->getAllSubTypes()[0][0].name);
		}
		else
		{
			string funcName=action->getReturnType()->getCompactString()+"=>"+getReturnType()->getCompactString();
			
			if (!prog->hasFunc(funcName))
			{
				Type argType=makeTuple({{"in", action->getReturnType()}}, true);
				
				prog->pushFunc(funcName, "", Void, argType, getReturnType());
					prog->declareVar("-out", getReturnType());
					
					auto outTypes=*getReturnType()->getAllSubTypes();
					
					if (action->getReturnType()->getType()==TypeBase::TUPLE)
					{
						auto inTypes=*action->getReturnType()->getAllSubTypes();
						
						for (int i=0; i<int(inTypes.size()); i++)
						{
							//if (inTypes[i].type==outTypes[i].type)
							{
								prog->name("-out");
								prog->code(".");
								prog->code(outTypes[i].name);
								prog->code(" = ");
								prog->name("in");
								prog->code(".");
								prog->code(inTypes[i].name);
								prog->endln();
							}
						}
					}
					else
					{
						prog->name("-out");
						prog->code(".");
						prog->code(outTypes[0].name);
						prog->code(" = ");
						prog->name("in");
						prog->endln();
					}
					
					prog->code("return ");
					prog->name("-out");
					prog->endln();
					
				prog->popFunc();
			}
			
			prog->name(funcName);
			prog->pushExpr();
				action->addToProg(prog);
			prog->popExpr();
		}
	}
	
private:
	
	Action action;
	
	friend GetElemFromTupleAction;
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
		size=typeOut->getSize();
		offset=typeIn->getSubType(name).offset;
	}
	
	string getDescription()
	{
		return str::putStringInTreeNodeBox(name);
		//return "get element from tuple";
	}
	
	void* execute(void* inLeft, void* inRight)
	{
		void* out=malloc(size);
		memcpy(out, (char*)inLeft+offset, size);
		return out;
	}
	
	void addToProg(Action inLeft, Action inRight, CppProgram* prog)
	{
		if (typeIn->getAllSubTypes()->size()==1)
		{
			inLeft->addToProg(prog);
			return;
		}
		
		MakeTupleAction * makeTupleAction=nullptr;
		
		if (typeid(*inLeft)==typeid(MakeTupleAction))
		{
			makeTupleAction=(MakeTupleAction *)&*inLeft;
		}
		
		else if (typeid(*inLeft)==typeid(CppTupleCastAction))
		{
			Action castAction=((CppTupleCastAction *)&*inLeft)->action;
			
			if (typeid(*castAction)==typeid(MakeTupleAction))
			{
				makeTupleAction=(MakeTupleAction *)&*castAction;
			}
		}
		
		if (makeTupleAction)
		{
			auto types=*inLeft->getReturnType()->getAllSubTypes();
			
			for (int i=0; i<int(types.size()); i++)
			{
				if (types[i].name==name)
				{
					makeTupleAction->sourceActions[i]->addToProg(prog);
					break;
				}
			}
		}
		else
		{
			/*
			if (inLeft->nameHint=="in" || inLeft->nameHint=="me")
			{
				prog->code(name);
			}
			else
			*/
			{
				inLeft->addToProg(prog);
				prog->code("."+name);
			}
		}
	}
	
private:
	Type typeIn;
	Type typeOut;
	int offset;
	size_t size;
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

Action cppTupleCastAction(Action actionIn, Type returnType)
{
	return Action(new CppTupleCastAction(actionIn, returnType));
}

