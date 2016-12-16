#include "../h/AstNode.h"

AstNode astVoid=AstNode(new AstVoid);

Type AstList::getReturnType(Namespace ns)
{
	if (nodes.empty())
	{
		return Void;
	}
	else
	{
		return nodes.back()->getReturnType(ns);
	}
}

Action AstList::getAction(Namespace ns)
{
	vector<Action> actions;
	
	for (auto i: nodes)
	{
		actions.push_back(i->getAction(ns));
	}
	
	Action out=listAction(actions);
	return out;
}

Type AstExpression::getReturnType(Namespace ns)
{
	return Void;
}

Action AstExpression::getAction(Namespace ns)
{
	return voidAction;
}
