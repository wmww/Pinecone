#include "../h/PineconeProgram.h"
#include "../h/LiteralElement.h"
#include "../h/IdentifierElement.h"
#include "../h/OperatorElement.h"
#include "../h/ErrorHandler.h"
#include "../h/Operator.h"

PineconeProgram::PineconeProgram()
{
	populatePineconeStdLib(getGlobalActionTable());
}

void PineconeProgram::cleanUp()
{
	globalFrame.clear();
}

void PineconeProgram::resolveProgram(bool printOutput)
{
	//initialProgramPopulation();
	
	//globalFrame.resolve(printOutput);
	
	lexString(inSource, inFileName, tokens);
	
	if (printOutput)
	{
		cout << endl << tableStringFromTokens(tokens, "lexer output") << endl;
	}
	
	astRoot=parseTokens(tokens, getGlobalActionTable());
	
	if (printOutput)
	{
		cout << endl << "parsed abstract syntax tree:\n" << astRoot->getDescription() << endl;
	}
}

ActionTablePtr PineconeProgram::getGlobalActionTable()
{
	return globalFrame.getElementList().getActionTable();
}

ElementPtr PineconeProgram::makeElement(ElementData data)
{
	switch (data.type)
	{
		case ElementData::IDENTIFIER: return IdentifierElement::makeNew(data);
			
		case ElementData::LITERAL: return LiteralElement::makeNew(data);
			
		case ElementData::OPERATOR: return OperatorElement::makeNew(data);
			
		default: return nullptr;
	}
	
	return nullptr;
}

void PineconeProgram::execute()
{
	free(astRoot->execute(nullptr, nullptr));
}

