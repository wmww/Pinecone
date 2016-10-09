
#include "../h/ElementList.h"
#include "../h/OperatorElement.h"
#include "../h/LiteralElement.h"
#include "../h/ErrorHandler.h"

ElementList::ElementList(ElementData dataIn, ElementList * parent): Element(dataIn)
{
	if (parent)
	{
		table=IdentifierTable(&(parent->table));
	}
}

void ElementList::appendElement(Element * in)
{
	elems.push_back(in);
}

void ElementList::clear()
{
	for (auto i=elems.begin(); i!=elems.end(); ++i)
		delete (*i);
	
	elems.clear();
	
	table.clear();
}

Type ElementList::getReturnType()
{
	if (elems.size()==1)
	{
		return (*elems.begin())->getReturnType();
	}
	else
	{
		return Type();
	}
}

void ElementList::structureByOperators()
{
	//parentheses
	
	for (auto i=elems.begin(); i!=elems.end(); ++i)
	{
		if ((*i)->getElemType()==ElementData::OPERATOR)
		{
			OperatorElement::OpType type=((OperatorElement *)(*i))->getType();
			
			if (type==OperatorElement::CLOSE)
			{
				error.log("extra closing parentheses", (*i)->getData(), SOURCE_ERROR);
			}
			
			else if (type==OperatorElement::OPEN)
			{
				int openNum=1;
				
				delete *i;
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
						OperatorElement::OpType type=((OperatorElement *)(*j))->getType();
						if (type==OperatorElement::OPEN)
						{
							++openNum;
						}
						else if (type==OperatorElement::CLOSE)
						{
							--openNum;
							
							if (openNum<=0)
							{
								if (j==i)
									++i;
								delete *j;
								j=elems.erase(j);
								break;
							}
						}
					}
					
					++j;
				}
				
				ElementList * subList=new ElementList((*i)->getData(), this);
				
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
			
			else if (type==OperatorElement::DOT)
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
	
	vector<OperatorElement::OpType> opTypes;
	
	opTypes.push_back(OperatorElement::PLUS);
	opTypes.push_back(OperatorElement::MINUS);
	absorbForOperators(opTypes);
	opTypes.clear();
	
	opTypes.push_back(OperatorElement::COLON);
	absorbForOperators(opTypes);
	opTypes.clear();
	
}

void ElementList::absorbForOperators(vector<OperatorElement::OpType> operators)
{
	for (auto i=elems.begin(); i!=elems.end(); ++i)
	{
		if ((*i)->getElemType()==ElementData::OPERATOR)
		{
			OperatorElement::OpType type=((OperatorElement *)(*i))->getType();
			
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
						
						((OperatorElement *)(*i))->setLeftInput(*j);
						((OperatorElement *)(*i))->setRightInput(*k);
						
						elems.erase(j);
						elems.erase(k);
					}
					
					break;
				}
			}
		}
	}
}

void ElementList::resolveIdentifiers()
{
	for (auto i=elems.begin(); i!=elems.end(); ++i)
	{
		(*i)->resolveIdentifiers(table);
	}
}

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

