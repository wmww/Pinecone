#include "../h/Namespace.h"
#include "../h/StackFrame.h"
#include "../h/msclStringFuncs.h"
#include "../h/ErrorHandler.h"

Namespace NamespaceData::makeRootNamespace()
{
	return Namespace(new NamespaceData(Namespace(nullptr), shared_ptr<StackFrame>(new StackFrame()), "root"));
}

Namespace NamespaceData::makeChild()
{
	return Namespace(new NamespaceData(shared_from_this(), stackFrame));
}

Namespace NamespaceData::makeChildAndFrame(string nameIn)
{
	return Namespace(new NamespaceData(shared_from_this(), shared_ptr<StackFrame>(new StackFrame()), nameIn));
}

NamespaceData::NamespaceData(Namespace parentIn, shared_ptr<StackFrame> stackFrameIn, string nameIn)
{
	parent=parentIn;
	stackFrame=stackFrameIn;
	myName=nameIn;
}

void NamespaceData::clear()
{
	allIds.clear();
	actions.clear();
	converters.clear();
	operators.clear();
	types.clear();
}

string NamespaceData::getString()
{
	string out;
	
	out+="normal functions:\n";
	
	for (auto i: actions)
	{
		out+="\t";
		
		out+=i.first;
		
		if (i.second.size()>1)
			out+=" (" + to_string(i.second.size()) + " overloads)";
		
		out+="\n";
	}
	
	out+="\nconverters:\n";
	
	for (auto i: converters)
	{
		out+="\t";
		
		out+=i.first->getString();
		
		if (i.second.size()>1)
			out+=" (" + to_string(i.second.size()) + " overloads)";
		
		out+="\n";
	}
	
	out+="\noperators:\n";
	
	for (auto i: operators)
	{
		out+="\t";
		out+=i.first->getText();
		
		if (i.second.size()>1)
			out+=" (" + to_string(i.second.size()) + " overloads)";
		
		out+="\n";
	}
	
	out+="\ntypes:\n";
	for (auto i: types)
	{
		out+="\t";
		out+=i.second->getString() + " (" + i.second->getString() + ")";
		out+="\n";
	}
	
	return out;
}

string NamespaceData::getStringWithParents()
{
	auto ptr=shared_from_this();
	
	string out;
	
	while (ptr)
	{
		out=putStringInBox(ptr->getString()+"\n"+out, ptr->myName);
		ptr=ptr->parent;
	}
	
	return out;
}

template<typename T, typename U>
void NamespaceData::addToMap(T key, U val, unordered_map<T, vector<U>>& hashMap)
{
	auto i=hashMap.find(key);
	
	if (i==hashMap.end())
	{
		hashMap[key]=vector<U>();
	}
	
	hashMap[key].push_back(val);
}

template<typename T, typename U>
void NamespaceData::getValuesFromMap(T key, vector<U>& out, unordered_map<T, vector<U>>& hashMap)
{
	auto matches=hashMap.find(key);
	
	if (matches!=hashMap.end())
	{
		out.insert(out.end(), matches->second.begin(), matches->second.end());
	}
	
	if (parent)
	{
		//	get the same map we were sent, but in the parent
		unordered_map<T, vector<U>>& parentMap=*((unordered_map<T, vector<U>>*)((char*)(&hashMap)-(char*)this+(char*)(&(*parent))));
		
		parent->getValuesFromMap(key, out, parentMap);
	}
}

void NamespaceData::setInput(Type left, Type right)
{
	if (parent && parent->getStackFrame()==stackFrame)
	{
		error.log("called "+FUNC+" on namespace that is not the root of a stack frame, thus it can not get input", INTERNAL_ERROR);
		return;
	}
	
	stackFrame->setInput(left, right);
	
	if (!left->isVoid())
	{
		string leftName="left";
		size_t leftOffset=stackFrame->getLeftOffset();
		Action leftGetAction=varGetAction(leftOffset, left, leftName);
		Action leftSetAction=varSetAction(leftOffset, left, leftName);
		addAction(leftGetAction, leftName);
		addAction(leftSetAction, leftName);
	}
	
	if (!right->isVoid())
	{
		string rightName="right";
		size_t rightOffset=stackFrame->getRightOffset();
		Action rightGetAction=varGetAction(rightOffset, right, rightName);
		Action rightSetAction=varSetAction(rightOffset, right, rightName);
		addAction(rightGetAction, rightName);
		addAction(rightSetAction, rightName);
	}
}

void NamespaceData::addVar(Type type, string name)
{
	size_t offset=stackFrame->getSize();
	stackFrame->addMember(type);
	
	Action getAction=varGetAction(offset, type, name);
	Action setAction=varSetAction(offset, type, name);
	addAction(getAction, name);
	addAction(setAction, name);
}

void NamespaceData::addAction(Action action, string id)
{
	Type type=getType(id);
	
	if (type!=Unknown)
	{
		if (action->getReturnType()==type)
		{
			addToMap(id, CONVERTER, allIds);
			addToMap(id, action, actions);
			addToMap(type, action, converters);
		}
		else
		{
			error.log("can not call an action '"+id+"' as there is already a type named that which does not match the return type", SOURCE_ERROR);
		}
	}
	else
	{
		addToMap(id, ACTION, allIds);
		addToMap(id, action, actions);
	}
}

void NamespaceData::addOperator(Action action, Operator op)
{
	addToMap(op->getText(), OPERATOR, allIds);
	addToMap(op, action, operators);
}

void NamespaceData::addType(Type type, string id)
{
	if (types.find(id)==types.end())
	{
		addToMap(type->getString(), TYPE, allIds);
		types[id]=type;
	}
	else
	{
		error.log("tried to create multiple types with the name '" + id + "'", INTERNAL_ERROR);
	}
}

Type NamespaceData::getType(string name)
{
	auto i=types.find(name);
	
	if (i==types.end())
	{
		if (parent)
		{
			return parent->getType(name);
		}
		else
		{
			return Unknown;
		}
	}
	else
	{
		return i->second;
	}
}

Action NamespaceData::getActionForTokenWithInput(Token token, Type left, Type right)
{
	vector<Action> matches;
	
	if (token->getOp())
	{
		getActions(token->getOp(), matches);
	}
	else
	{
		getActions(token->getText(), matches);
	}
	
	//error.log("found "+to_string(matches.size())+" overloads for "+token->getText(), JSYK, token);
	
	Action selection=findActionWithInput(matches, left, right);
	
	if (selection)
		return selection;
	else
		throw IdNotFoundError(token->getText(), matches.size()>0, shared_from_this());
}

Action NamespaceData::getActionForTokenWithInput(Token token, Action left, Action right)
{
	Action selection=getActionForTokenWithInput(token, left->getReturnType(), right->getReturnType());
	
	if (selection!=voidAction)
	{
		return branchAction(left, selection, right);
	}
	else
	{
		return voidAction;
	} 
}

Action NamespaceData::getActionConvertedToType(Action actionIn, Type outType)
{
	if (actionIn->getReturnType()==outType)
		return actionIn;
	
	vector<Action> typeConverters;
	
	getConvertersToType(outType, typeConverters);
	
	for (auto i: typeConverters)
	{
		if (i->getInRightType()==actionIn->getReturnType())
		{
			return branchAction(voidAction, i, actionIn);
		}
	}
	
	return voidAction;
}

void NamespaceData::getActions(string text, vector<Action>& out)
{
	getValuesFromMap(text, out, actions);
}

void NamespaceData::getActions(Operator op, vector<Action>& out)
{
	getValuesFromMap(op, out, operators);
}

void NamespaceData::getConvertersToType(Type typeIn, vector<Action>& out)
{
	getValuesFromMap(typeIn, out, converters);
}

Action NamespaceData::findActionWithInput(vector<Action>& actionsIn, Type leftInType, Type rightInType)
{
	//error.log																	\
	(																			\
		string()+																\
			"findActionWithInput called."+										\
			"\n\tinput:\n"+														\
			[&]()->string														\
			{																	\
				string out;														\
				for (auto i: actionsIn)											\
					out+="\t\t"+i->getTypesString()+"\n";						\
				return out;														\
			}()+																\
			"\n\ttarget:\n"+													\
			"\t\t"+leftInType->getString()+"."+rightInType->getString()+"\n",	\
		JSYK																	\
	);																			\
	
	Action match(nullptr);
	
	for (auto i: actionsIn)
	{
		//if (i->getInLeftType()==leftInType && i->getInRightType()==rightInType)
		if (i->getInLeftType()->matches(leftInType) && i->getInRightType()->matches(rightInType))
		{
			if (match)
			{
				error.log(FUNC+"found too many overloads", INTERNAL_ERROR);
				return Action(nullptr);
			}
			else
			{
				match=i;
			}
		}
	}
	
	if (match)
	{
		return match;
	}
	else
	{
		return Action(nullptr);
	}
}


