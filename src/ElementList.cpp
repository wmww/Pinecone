
#include "../h/ElementList.h"
#include "../h/OperatorElement.h"
#include "../h/LiteralElement.h"
#include "../h/ErrorHandler.h"
#include "../h/ListAction.h"
#include "../h/StackFrame.h"

using std::prev;
using std::next;

ElementList::ElementList(ElementData dataIn, ElementList& parentList): Element(dataIn)
{
	table=std::unique_ptr<ActionTable>(new ActionTable(parentList.getActionTable()));
}

ElementList::ElementList(ElementData dataIn, StackFrame& frame): Element(dataIn)
{
	table=std::unique_ptr<ActionTable>(new ActionTable(&frame));
}

void ElementList::appendElement(ElementPtr in)
{
	elems.push_back(in);
}

void ElementList::clear()
{
	elems.clear();
	
	table->clear();
}

void ElementList::structureByOperators()
{
	//parentheses and . as part of Dub literal
	for (auto i=elems.begin(); i!=elems.end(); ++i)
	{
		if ((*i)->getElemType()==ElementData::OPERATOR)
		{
			OperatorType type=((OperatorElement *)&(**i))->getType();
			
			if (type==OP_CLOSE)
			{
				error.log("extra closing parentheses", (*i)->getData(), SOURCE_ERROR);
			}
			
			else if (type==OP_OPEN)
			{
				int openNum=1;
				
				i=elems.erase(i);
				
				auto j=i;
				
				while (true)
				{
					if (j==elems.end())
					{
						error.log("could not find closing parentheses", (*i)->getData(), SOURCE_ERROR);
						break;
					}
					
					if ((*j)->getElemType()==ElementData::OPERATOR)
					{
						OperatorType type=((OperatorElement *)&(**j))->getType();
						if (type==OP_OPEN)
						{
							++openNum;
						}
						else if (type==OP_CLOSE)
						{
							--openNum;
							
							if (openNum<=0)
							{
								if (j==i)
									++i;
								j=elems.erase(j);
								break;
							}
						}
					}
					
					++j;
				}
				
				shared_ptr<ElementList> subList(new ElementList((*i)->getData(), *this));
				
				auto k=i;
				
				while (k!=j)
				{
					subList->appendElement(*k);
					k=elems.erase(k);
				}
				
				i=j;
				
				elems.insert(i, subList);
				
				subList->structureByOperators();
			}
			
			else if (type==OP_DOT)
			{
				auto j=i;
				
				++j;
				
				if (j!=elems.end())
				{
					if ((*j)->getElemType()==ElementData::LITERAL)
					{
						if (((LiteralElement*)&(**j))->getType()==Int)
						{
							ElementData data0=(*i)->getData();
							ElementData data1=(*j)->getData();
							
							elems.insert(i, LiteralElement::makeNew(ElementData(data0.text+data1.text, data0.file, data0.line, ElementData::LITERAL)));
							
							i=elems.erase(i);
							i=elems.erase(i);
							
							--i;
							
							if (i!=elems.begin())
							{
								j=i;
								--j;
								
								if ((*j)->getElemType()==ElementData::LITERAL)
								{
									if (((LiteralElement*)&(**j))->getType()==Int)
									{
										data0=(*j)->getData();
										data1=(*i)->getData();
										
										elems.insert(j, LiteralElement::makeNew(ElementData(data0.text+data1.text, data0.file, data0.line, ElementData::LITERAL)));
										
										j=elems.erase(j);
										j=elems.erase(j);
										
										--j;
										
										i=j;
									}
								}
							}
						}
					}
				}
			}
		}
	}
	
	vector<OperatorType> opTypes;
	
	opTypes.push_back(OP_COLON);
	absorbForOperators(opTypes, true, false, true);
	opTypes.clear();
	
	opTypes.push_back(OP_PLUS);
	opTypes.push_back(OP_MINUS);
	absorbForOperators(opTypes, true, true, false);
	opTypes.clear();
	
	opTypes.push_back(OP_COLON);
	absorbForOperators(opTypes, false, true, true);
	opTypes.clear();
	
}

void ElementList::absorbForOperators(vector<OperatorType> operators, bool absorbLeft, bool absorbRight, bool backwords)
{
	if (!absorbLeft && !absorbRight)
	{
		error.log(string() + "called " + __FUNCTION__ + " with both absorbLeft and absorbRight false, which is kinda pointless", INTERNAL_ERROR);
		return;
	}
	
	if (elems.empty())
		return;
	
	auto i=backwords?prev(elems.end()):elems.begin();
	
	while (true)
	{
		if ((*i)->getElemType()==ElementData::OPERATOR)
		{
			
			OperatorType type=((OperatorElement *)&(**i))->getType();
			
			for (auto l=operators.begin(); l!=operators.end(); ++l)
			{
				if (type==*l)
				{
					auto j=i;
					
					if (absorbLeft)
					{
						if (j!=elems.begin())
						{
							j--;
							
							((OperatorElement *)&(**i))->setLeftInput(*j);
							
							elems.erase(j);
						}
						//else the scope starts with this operator, and thus it cant have a left input
					}
					
					j=i;
					
					if (absorbRight)
					{
						j++;
						
						if (j!=elems.end())
						{
							((OperatorElement *)&(**i))->setRightInput(*j);
							
							elems.erase(j);
						}
						//else the scope ends with this operator, and thus it cant have a left input
					}
					
					break;
				}
			}
		}
		
		if (i==(backwords?elems.begin():prev(elems.end())))
			break;
		
		if (backwords)
			--i;
		else
			++i;
			
	}
}

ActionPtr ElementList::resolveActions()
{
	list<ActionPtr> data;
	
	for (auto i=elems.begin(); i!=elems.end(); ++i)
	{
		data.push_back((*i)->resolveActions(table));
	}
	
	return ActionPtr(new ListAction(data));
}

/*
void* ElementList::execute()
{
	if (elems.size()!=1)
	{
		for (auto i=elems.begin(); i!=elems.end(); ++i)
		{
			(*i)->getReturnType().deleteVoidPtr((*i)->execute());
		}
		
		return nullptr;
	}
	else
	{
		return elems.back()->execute();
	}
}
*/

void ElementList::printToString(string& in, int depth)
{
	if (depth<0)
	{
		for (auto i=elems.begin(); i!=elems.end();)
		{
			(*i)->printToString(in, 0);
			
			++i;
			
			if (i!=elems.end())
				in+="\n";
		}

	}
	else
	{
		printIndentationToString(in, depth);
		in+="{\n";
		
		for (auto i=elems.begin(); i!=elems.end();)
		{
			(*i)->printToString(in, depth+2);
			
			++i;
			
			if (i!=elems.end())
				in+="\n";
		}
		
		printIndentationToString(in, depth);
		in+="}\n";
	}
}

