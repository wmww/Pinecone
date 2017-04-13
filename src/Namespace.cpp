#include "../h/Namespace.h"
#include "../h/StackFrame.h"
#include "../h/msclStringFuncs.h"
#include "../h/ErrorHandler.h"

class ActionWrapperNode: public AstNodeBase
{
public:
	
	static unique_ptr<ActionWrapperNode> make(Action actionIn) {
		auto out = unique_ptr<ActionWrapperNode>(new ActionWrapperNode);
		out->inLeftType=actionIn->getInLeftType();
		out->inRightType=actionIn->getInRightType();
		out->returnType=actionIn->getReturnType();
		out->action=actionIn;
		out->dynamic=true; // shouldn't matter
		out->ns=nullptr; // shouldn't matter
		out->inputHasBeenSet=true;
		return out;
	}
	
	string getString() {return "action wrapper node";}
	
	AstNode makeCopy(bool copyCache)
	{
		auto out=new ActionWrapperNode;
		copyToNode(out, true);
		return AstNode(out);
	}
	
	void resolveAction()
	{
		throw PineconeError("ActionWrapperNode::resolveAction called, which it shouldn't have been", INTERNAL_ERROR);
	}
	
	Token getToken() {return nullptr;}
};

string convertToString(Operator in) {return in->getText();}
string convertToString(Type in) {return in->getString();}
string convertToString(string in) {return in;}

void NamespaceData::ActionMap::add(string key, AstNode node)
{
	auto i=nodes.find(key);
	
	if (i==nodes.end())
	{
		nodes[key]=vector<AstNode>();
	}
	
	nodes[key].push_back(move(node));
}

void NamespaceData::ActionMap::add(string key, Action action)
{
	add(key, ActionWrapperNode::make(action));
	
	/*
	auto i=actions.find(key);
	
	if (i==actions.end())
	{
		actions[key]=vector<Action>();
	}
	
	actions[key].push_back(action);
	*/
}

void NamespaceData::ActionMap::get(string key, vector<AstNodeBase*>& out)
{
	auto matches=nodes.find(key);
	
	if (matches!=nodes.end())
	{
		for (unsigned i=0; i<matches->second.size(); i++)
		{
			out.push_back(&*matches->second[i]);
		}
	}
}

/*
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
*/

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
	if (parent && parent->getStackFrame()==stackFrame)
	{
		error.log("called "+FUNC+" on namespace that is not the root of a stack frame, thus it can not get input", INTERNAL_ERROR);
		return;
	}
	
	stackFrame->setInput(left, right);
	
	//if (!left->isVoid())
	if (left->isCreatable())
	{
		string leftName="me";
		size_t leftOffset=stackFrame->getLeftOffset();
		Action leftGetAction=varGetAction(leftOffset, left, leftName);
		Action leftSetAction=varSetAction(leftOffset, left, leftName);
		addAction(leftGetAction, leftName);
		addAction(leftSetAction, leftName);
	}
	
	//if (!right->isVoid())
	if (right->isCreatable())
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
			throw PineconeError("you tried to make multiple types with the name '"+id+"'", SOURCE_ERROR);
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
	actions.add(op->getText(), action);
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
	if (node->nameHint.empty())
		node->nameHint=id;
	
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
			//node->setInput(shared_from_this(), false, Void, Void);
			
			if ( // if the left or the right is a Whatev and the types match up
				typeid(*node)==typeid(AstFuncBody) && ((AstFuncBody*)&*node)->isWhatev()
			){
				whatevActions.add(id, move(node));
			}
			else
			{
				actions.add(id, move(node));
			}
			
			//addToMap(id, ACTION, allIds);
			//addToMap(id, action, actions);
		}
	}
}

void NamespaceData::addOperator(AstNode node, Operator op)
{
	actions.add(op->getText(), move(node));
	//addToMap(op->getText(), OPERATOR, allIds);
	//addToMap(op, action, operators);
}

Type NamespaceData::getType(string name)
{
	vector<AstNodeBase*> results;
	
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
		throw PineconeError("node returning non meta type stored in namespace type map for type '"+name+"'", INTERNAL_ERROR);
	}
	else
	{
		return results[0]->getReturnType()->getSubType();
	}
}

Action NamespaceData::getActionForTokenWithInput(Token token, Type left, Type right, bool dynamic)
{
	vector<Action> matches;
	vector<AstNodeBase*> nodes;
	
	string searchText;
	
	if (token->getOp())
	{
		searchText=token->getOp()->getText();
	}
	else
	{
		searchText=token->getText();
	}
	
	getMatches(nodes, searchText, true, dynamic, false);
	
	for (auto i: nodes)
	{
		Action action=i->getAction();
		
		if (action->getInLeftType()->matches(left) && action->getInRightType()->matches(right))
			matches.push_back(action);
	}
	
	if (!matches.empty())
	{
		if (matches.size() == 1)
		{
			return matches[0];
		}
		else
		{
			throw IdNotFoundError(searchText, true, shared_from_this());
		}
	}
	
	getMatches(nodes, searchText, false, false, true);
	
	for (auto i: nodes)
	{
		if (typeid(*i)!=typeid(AstFuncBody))
			throw PineconeError("AST node other then AstFuncBody made it into the whatev table", INTERNAL_ERROR, i->getToken());
		
		if ( // getSubType is needed because leftTypeNode is a type node and a type node always returns a metatype
			((AstFuncBody*)i)->leftTypeNode->getReturnType()->getSubType()->matches(left)
			&&
			((AstFuncBody*)i)->rightTypeNode->getReturnType()->getSubType()->matches(right)
		) {
			AstNode instance=((AstFuncBody*)i)->makeNonWhatevCopy(left, right);
			matches.push_back(instance->getAction());
			actions.add(token->getText(), move(instance));
		}
	}
	
	if (!matches.empty())
	{
		if (matches.size() == 1)
		{
			return matches[0];
		}
		else
		{
			throw IdNotFoundError(searchText, true, shared_from_this());
		}
	}
	
	throw IdNotFoundError(searchText, false, shared_from_this());
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

void NamespaceData::getMatches(vector<AstNodeBase*>& out, string text, bool checkActions, bool checkDynamic, bool checkWhatev)
{
	if (checkActions)
	{
		actions.get(text, out);
	}
	
	if (checkDynamic)
	{
		dynamicActions.get(text, out);
	}
	
	if (checkWhatev)
	{
		whatevActions.get(text, out);
	}
	
	if (parent)
		parent->getMatches(out, text, checkActions, checkDynamic, checkWhatev);
}

void NamespaceData::getActions(string text, vector<Action>& out, bool dynamic)
{
	vector<AstNodeBase*> nodes;
	
	actions.get(text, nodes);
	
	if (dynamic)
	{
		dynamicActions.get(text, nodes);
	}
	
	for (int i=0; i<int(nodes.size()); i++)
	{
		out.push_back(nodes[i]->getAction());
	}
	
	if (parent)
		parent->getActions(text, out, dynamic);
}

void NamespaceData::getActions(Operator op, vector<Action>& out)
{
	vector<AstNodeBase*> nodes;
	
	actions.get(op->getText(), nodes);
	
	for (int i=0; i<int(nodes.size()); i++)
	{
		out.push_back(nodes[i]->getAction());
	}
	
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


