
#include "../h/ElementList.h"
#include "../h/OperatorElement.h"
#include "../h/LiteralElement.h"
#include "../h/ErrorHandler.h"
#include "../h/ListAction.h"
#include "../h/StackFrame.h"

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

Type ElementList::getReturnType()
{
	return elems.back()->getReturnType();
}

void ElementList::structureByOperators()
{
	//parentheses
	
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
						if ((*j)->getReturnType()==Type(Type::INT))
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
									if ((*j)->getReturnType()==Type(Type::INT))
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
	
	opTypes.push_back(OP_PLUS);
	opTypes.push_back(OP_MINUS);
	absorbForOperators(opTypes);
	opTypes.clear();
	
	opTypes.push_back(OP_COLON);
	absorbForOperators(opTypes);
	opTypes.clear();
	
}

void ElementList::absorbForOperators(vector<OperatorType> operators)
{
	for (auto i=elems.begin(); i!=elems.end(); ++i)
	{
		if ((*i)->getElemType()==ElementData::OPERATOR)
		{
			OperatorType type=((OperatorElement *)&(**i))->getType();
			
			for (auto l=operators.begin(); l!=operators.end(); ++l)
			{
				if (type==*l)
				{
					auto j=i, k=i;
					
					++k;
					
					if (j==elems.begin())
					{
						error.log("scope started with " + OperatorElement::toString(type), (*i)->getData(), SOURCE_ERROR);
					}
					else if (k==elems.end())
					{
						error.log("scope ended with " + OperatorElement::toString(type), (*i)->getData(), SOURCE_ERROR);
					}
					else
					{
						--j;
						
						((OperatorElement *)&(**i))->setLeftInput(*j);
						((OperatorElement *)&(**i))->setRightInput(*k);
						
						elems.erase(j);
						elems.erase(k);
					}
					
					break;
				}
			}
		}
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

