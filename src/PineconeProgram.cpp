#include "../h/PineconeProgram.h"
#include "../h/ErrorHandler.h"
#include "../h/Operator.h"
#include "../h/AllOperators.h"

void populatePineconeStdLib();
void lexString(SourceFile& file, vector<Token>& tokens);
Action parseFunction(const vector<Token>& tokens, int left, int right, Type leftInType, Type rightInType);

extern Namespace stdLibNamespace;

PineconeProgram::PineconeProgram()
{
	globalFrame=shared_ptr<StackFrame>(new StackFrame);
}

void PineconeProgram::cleanUp()
{
	globalFrame=nullptr;
}

void PineconeProgram::resolveProgram(string inFilename, bool printOutput)
{
	AllOperators::init();
	populatePineconeStdLib();
	
	//initialProgramPopulation();
	
	//globalFrame.resolve(printOutput);
	
	file=SourceFile(inFilename);
	
	if (printOutput)
	{
		cout << endl << endl << file.getBoxedString() << endl;
	}
	
	lexString(file, tokens);
	
	if (printOutput)
	{
		cout << endl << tableStringFromTokens(tokens, "lexer output") << endl;
	}
	
	//astRoot=parseFunction(tokens, 0, tokens.size()-1, Void, Void);
	
	try
	{
		astRoot=astNodeFromTokens(tokens);
	}
	catch (PineconeError err)
	{
		err.log();
		astRoot=AstVoid::make();
	}
	
	if (printOutput)
	{
		cout << endl << putStringInBox(astRoot->getString(), "parsed abstract syntax tree") << endl;
	}
	
	try
	{
		astRoot->setInput(stdLibNamespace, Void, Void);
	}
	catch (PineconeError err)
	{
		err.log();
		astRoot=AstVoid::make();
	}
	
	try
	{
		actionRoot=functionAction(astRoot->getAction(), globalFrame);
		
		cout << endl << putStringInBox(actionRoot->getDescription(), "parsed action tree") << endl;
	}
	catch (PineconeError err)
	{
		err.log();
	}
	
	/*if (printOutput)
	{
		cout << endl << "C source code:\n" << astRoot->getCSource() << endl;
	}*/
	
}

void PineconeProgram::execute()
{
	try
	{
		free(actionRoot->execute(nullptr, nullptr));
	}
	catch (PineconeError err)
	{
		err.log();
		cout << endl << ">>>>>>    program aborted due to error    <<<<<<" << endl;
	}
}

