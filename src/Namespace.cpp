#include "../h/Namespace.h"
#include "../h/StackFrame.h"
#include "../h/msclStringFuncs.h"
#include "../h/ErrorHandler.h"

template<typename KEY>
void NamespaceData::ActionMap<KEY>::add(KEY key, AstNode node)
{
	auto i=nodes.find(key);
	
	if (i==nodes.end())
	{
		nodes[key]=vector<AstNode>();
	}
	
	nodes[key].push_back(move(node));
}

template<typename KEY>
void NamespaceData::ActionMap<KEY>::add(KEY key, Action action)
{
	auto i=actions.find(key);
	
	if (i==actions.end())
	{
		actions[key]=vector<Action>();
	}
	
	actions[key].push_back(action);
}

string convertToString(Operator in) {return in->getText();}
string convertToString(Type in) {return in->getString();}
string convertToString(string in) {return in;}

template<typename KEY>
void NamespaceData::ActionMap<KEY>::get(KEY key, vector<Action>& out)
{
	auto matches1=nodes.find(key);
	
	if (matches1!=nodes.end())
	{
		for (unsigned i=0; i<matches1->second.size(); i++)
		{
			Action valAction;
			
			if (matches1->second[i]->isType())
			{
				valAction=typeGetAction(matches1->second[i]->getReturnType());
			}
			else
			{
				Action resultAction=matches1->second[i]->getAction();
				
				if (resultAction->isFunction())
				{
					valAction=resultAction;
				}
				else
				{
					void* val=resultAction->execute(nullptr, nullptr);
					valAction=constGetAction(val, resultAction->getReturnType(), convertToString(key));
				}
			}
			
			add(key, valAction);
		}
		
		nodes.erase(key);
	}
	
	auto matches2=actions.find(key);
	
	if (matches2!=actions.end())
	{
		out.insert(out.end(), matches2->second.begin(), matches2->second.end());
	}
}

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

string NamespaceData::getString()
{
	/*string out;
	
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
	
	return out;*/
	
	return "NamespaceData::getString not yet implemented";
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

/* // addToMap templates
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
*/

void NamespaceData::setInput(Type left, Type right)
{
	//error.log("input on namespace set to "+left->getString()+" . "+right->getString(), JSYK);
	
	if (parent && parent->getStackFrame()==stackFrame)
	{
		error.log("called "+FUNC+" on namespace that is not the root of a stack frame, thus it can not get input", INTERNAL_ERROR);
		return;
	}
	
	stackFrame->setInput(left, right);
	
	if (!left->isVoid())
	{
		string leftName="me";
		size_t leftOffset=stackFrame->getLeftOffset();
		Action leftGetAction=varGetAction(leftOffset, left, leftName);
		Action leftSetAction=varSetAction(leftOffset, left, leftName);
		addAction(leftGetAction, leftName);
		addAction(leftSetAction, leftName);
	}
	
	if (!right->isVoid())
	{
		string rightName="in";
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
	
	Action getAction;
	Action setAction;
	
	Namespace top=shared_from_this();
	
	while(top->parent)
	{
		top=top->parent;
	}
	
	if (stackFrame!=top->stackFrame)
	{
		getAction=varGetAction(offset, type, name);
		setAction=varSetAction(offset, type, name);
	}
	else
	{
		getAction=globalGetAction(offset, type, name);
		setAction=globalSetAction(offset, type, name);
	}
	
	dynamicActions.add(name, getAction);
	dynamicActions.add(name, setAction);
}

void NamespaceData::addAction(Action action, string id)
{
	if (action->getReturnType()->getType()==TypeBase::METATYPE)
	{
		try
		{
			getType(id);
			throw PineconeError("tried to make multiple types with the name '"+id+"'", SOURCE_ERROR);
		}
		catch (IdNotFoundError err) {}
		
		types.add(id, action);
	}
	else
	{
		/*try
		{
			Type type=getType(id);
			
			if (action->getReturnType()==type)
			{
				actions.add(id, action);
				converters.add(type, action)
				//addToMap(id, CONVERTER, allIds);
				addToMap(id, action, actions);
				addToMap(type, action, converters);
			}
			else
			{
				throw PineconeError("can not call an action '"+id+"' as there is already a type named that which does not match the return type", SOURCE_ERROR);
			}
		}
		catch (IdNotFoundError err)*/
		{
			actions.add(id, action);
			//addToMap(id, ACTION, allIds);
			//addToMap(id, action, actions);
		}
	}
}

void NamespaceData::addOperator(Action action, Operator op)
{
	operators.add(op, action);
	//addToMap(op->getText(), OPERATOR, allIds);
	//addToMap(op, action, operators);
}

void NamespaceData::addType(Type type, string id)
{
	if (type->nameHint.empty())
		type->nameHint=id;
	addAction(typeGetAction(type), id);
}

void NamespaceData::addAction(AstNode node, string id)
{
	if (node->isType())
	{
		types.add(id, move(node));
	}
	else
	{
		/*try
		{
			Type type=getType(id);
			
			if (action->getReturnType()==type)
			{
				actions.add(id, action);
				converters.add(type, action)
				//addToMap(id, CONVERTER, allIds);
				addToMap(id, action, actions);
				addToMap(type, action, converters);
			}
			else
			{
				throw PineconeError("can not call an action '"+id+"' as there is already a type named that which does not match the return type", SOURCE_ERROR);
			}
		}
		catch (IdNotFoundError err)*/
		{
			actions.add(id, move(node));
			//addToMap(id, ACTION, allIds);
			//addToMap(id, action, actions);
		}
	}
}

void NamespaceData::addOperator(AstNode node, Operator op)
{
	operators.add(op, move(node));
	//addToMap(op->getText(), OPERATOR, allIds);
	//addToMap(op, action, operators);
}

Type NamespaceData::getType(string name)
{
	vector<Action> results;
	
	types.get(name, results);
	
	if (results.empty())
	{
		if (parent)
			return parent->getType(name);
		else
			throw IdNotFoundError(name, false, shared_from_this());
	}
	else if (results.size()!=1)
	{
		throw PineconeError("namespace has multiple defenitions of the same type '"+name+"'", INTERNAL_ERROR);
	}
	else if (results[0]->getReturnType()->getType()!=TypeBase::METATYPE)
	{
		throw PineconeError("action returning non meta type stored in namespace type map for type '"+name+"'", INTERNAL_ERROR);
	}
	else
	{
		return results[0]->getReturnType()->getSubType();
	}
}

Action NamespaceData::getActionForTokenWithInput(Token token, Type left, Type right, bool dynamic)
{
	vector<Action> matches;
	
	if (token->getOp())
	{
		getActions(token->getOp(), matches);
	}
	else
	{
		getActions(token->getText(), matches, dynamic);
	}
	
	//error.log("found "+to_string(matches.size())+" overloads for "+token->getText(), JSYK, token);
	
	Action selection=findActionWithInput(matches, left, right);
	
	if (selection)
		return selection;
	else
		throw IdNotFoundError(token->getText(), matches.size()>0, shared_from_this());
}

Action NamespaceData::getActionForTokenWithInput(Token token, Action left, Action right, bool dynamic)
{
	Action selection=getActionForTokenWithInput(token, left->getReturnType(), right->getReturnType(), dynamic);
	
	if (selection!=voidAction)
	{
		return branchAction(left, selection, right);
	}
	else
	{
		return voidAction;
	} 
}

/*Action NamespaceData::getActionConvertedToType(Action actionIn, Type outType)
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
}*/

void NamespaceData::getActions(string text, vector<Action>& out, bool dynamic)
{
	actions.get(text, out);
	
	if (dynamic)
	{
		dynamicActions.get(text, out);
	}
	
	if (parent)
		parent->getActions(text, out, dynamic);
}

void NamespaceData::getActions(Operator op, vector<Action>& out)
{
	operators.get(op, out);
	
	if (parent)
		parent->getActions(op, out);
}

/*
void NamespaceData::getConvertersToType(Type typeIn, vector<Action>& out)
{
	getValuesFromMap(typeIn, out, converters);
}
*/

Action NamespaceData::findActionWithInput(vector<Action>& actionsIn, Type leftInType, Type rightInType)
{
	/*
	error.log
	(
		string()+
			"findActionWithInput called."+
			"\n\tinput:\n"+
			[&]()->string
			{
				string out;
				for (auto i: actionsIn)
					out+="\t\t"+i->getTypesString()+"\n";
				return out;
			}()+
			"\n\ttarget:\n"+
			"\t\t"+leftInType->getString()+"."+rightInType->getString()+"\n",
		JSYK
	);
	*/
	
	Action match(nullptr);
	
	for (auto i: actionsIn)
	{
		//if (i->getInLeftType()==leftInType && i->getInRightType()==rightInType)
		if (i->getInLeftType()->matches(leftInType) && i->getInRightType()->matches(rightInType))
		{
			if (match)
			{
				error.log(FUNC+" found too many overloads including " + match->getDescription() + " and " + i->getDescription(), INTERNAL_ERROR);
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


