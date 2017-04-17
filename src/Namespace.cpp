#include "../h/Namespace.h"
#include "../h/StackFrame.h"
#include "../h/msclStringFuncs.h"
#include "../h/ErrorHandler.h"

void NamespaceData::IdMap::add(string key, AstNode node)
{
	auto i=nodes.find(key);
	
	if (i==nodes.end())
	{
		nodes[key]=vector<AstNode>();
	}
	
	nodes[key].push_back(move(node));
}

void NamespaceData::IdMap::get(string key, vector<AstNodeBase*>& out)
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
		addNode(AstActionWrapper::make(leftGetAction), leftName);
		addNode(AstActionWrapper::make(leftSetAction), leftName);
	}
	
	//if (!right->isVoid())
	if (right->isCreatable())
	{
		string rightName="in";
		size_t rightOffset=stackFrame->getRightOffset();
		Action rightGetAction=varGetAction(rightOffset, right, rightName);
		Action rightSetAction=varSetAction(rightOffset, right, rightName);
		addNode(AstActionWrapper::make(rightGetAction), rightName);
		addNode(AstActionWrapper::make(rightSetAction), rightName);
	}
}

Action NamespaceData::addVar(Type type, string name)
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
	
	dynamicActions.add(name, AstActionWrapper::make(getAction));
	dynamicActions.add(name, AstActionWrapper::make(setAction));
	
	return setAction;
}

void NamespaceData::addNode(AstNode node, string id)
{
	if (node->nameHint.empty())
		node->nameHint=id;
	
	if (node->isType())
	{
		types.add(id, move(node));
	}
	else
	{
		// if the left or the right is a Whatev and the types match up
		if (typeid(*node)==typeid(AstFuncBody) && ((AstFuncBody*)&*node)->isWhatev())
		{
			whatevActions.add(id, move(node));
		}
		else
		{
			actions.add(id, move(node));
		}
	}
}

Type NamespaceData::getType(string name, bool throwSourceError, Token tokenForError)
{
	vector<AstNodeBase*> results;
	
	types.get(name, results);
	
	if (results.empty())
	{
		if (parent)
			return parent->getType(name, throwSourceError, tokenForError);
		else if (throwSourceError)
			throw PineconeError("'"+name+"' type not found", SOURCE_ERROR);
		else
			return nullptr;
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

Action NamespaceData::getActionForTokenWithInput(Token token, Type left, Type right, bool dynamic, bool throwSourceError, Token tokenForError)
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
		else if (throwSourceError)
		{
			throw PineconeError("multiple matching instances of '"+token->getText()+"' found", SOURCE_ERROR, tokenForError);
		}
		else
		{
			return nullptr;
		}
	}
	
	nodes.clear();
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
		else if (throwSourceError)
		{
			throw PineconeError("multiple whatev instanes of '"+token->getText()+"' found", SOURCE_ERROR, tokenForError);
		}
		else
		{
			return nullptr;
		}
	}
	
	if (dynamic && token->getType()==TokenData::IDENTIFIER && left->isVoid() && right->isCreatable())
	{
		return addVar(right, token->getText());
	}
	
	if (throwSourceError)
	{
		throw PineconeError("'"+token->getText()+"' not found", SOURCE_ERROR, tokenForError);
	}
	else
	{
		return nullptr;
	}
}

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

