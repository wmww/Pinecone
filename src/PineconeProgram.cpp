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
	
}

void PineconeProgram::cleanUp()
{
	
}

void PineconeProgram::resolveProgram(string inFilename, bool printOutput)
{
	AllOperators::init();
	populatePineconeStdLib();
	
	//initialProgramPopulation();
	
	//globalFrame.resolve(printOutput);
	
	if (!error.getIfErrorLogged())
	{
		file=SourceFile(inFilename, printOutput);
		
		if (printOutput)
		{
			cout << endl << endl << file.getBoxedString() << endl;
		}
	}
	
	if (!error.getIfErrorLogged())
	{
		lexString(file, tokens);
		
		if (printOutput)
		{
			cout << endl << tableStringFromTokens(tokens, "lexer output") << endl;
		}
	}
	
	//astRoot=parseFunction(tokens, 0, tokens.size()-1, Void, Void);
	
	if (!error.getIfErrorLogged())
	{
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
			cout << endl << putStringInBox(astRoot->getString(), "abstract syntax tree") << endl;
		}
	}
	
	if (!error.getIfErrorLogged())
	{
		try
		{
			astRoot->setInput(stdLibNamespace, true, Void, Void);
		}
		catch (PineconeError err)
		{
			err.log();
			astRoot=AstVoid::make();
		}
		
		try
		{
			actionRoot=functionAction(astRoot->getAction(), stdLibNamespace->getStackFrame());
			
			if (printOutput)
			{
				cout << endl << putStringInBox(actionRoot->getDescription(), "action tree") << endl;
			}
		}
		catch (PineconeError err)
		{
			err.log();
		}
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

