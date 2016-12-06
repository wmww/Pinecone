#include "../h/PineconeProgram.h"
#include "../h/ErrorHandler.h"
#include "../h/Operator.h"
#include "../h/AllOperators.h"

void populatePineconeStdLib();
void lexString(string text, string filename, vector<Token>& tokens);
Action parseFunction(const vector<Token>& tokens, int left, int right, Type leftInType, Type rightInType);

PineconeProgram::PineconeProgram()
{
	
}

void PineconeProgram::cleanUp()
{
	globalFrame=StackFrame();
}

void PineconeProgram::resolveProgram(bool printOutput)
{
	AllOperators::init();
	populatePineconeStdLib();
	
	//initialProgramPopulation();
	
	//globalFrame.resolve(printOutput);
	
	lexString(inSource, inFileName, tokens);
	
	if (printOutput)
	{
		cout << endl << tableStringFromTokens(tokens, "lexer output") << endl;
	}
	
	astRoot=parseFunction(tokens, 0, tokens.size()-1, Void, Void);
	
	if (printOutput)
	{
		cout << endl << "parsed abstract syntax tree:\n" << astRoot->getDescription() << endl;
	}
	
	/*if (printOutput)
	{
		cout << endl << "C source code:\n" << astRoot->getCSource() << endl;
	}*/
	
}

void PineconeProgram::execute()
{
	free(astRoot->execute(nullptr, nullptr));
}

