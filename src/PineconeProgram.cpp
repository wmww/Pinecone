#include "../h/PineconeProgram.h"
#include "../h/ErrorHandler.h"
#include "../h/Operator.h"
#include "../h/AllOperators.h"
#include "../h/CppProgram.h"

void populatePineconeStdLib();
void lexString(shared_ptr<SourceFile> file, vector<Token>& tokens);
Action parseFunction(const vector<Token>& tokens, int left, int right, Type leftInType, Type rightInType);

extern Namespace globalNamespace;

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
		try
		{
			file=shared_ptr<SourceFile>(new SourceFile(inFilename, printOutput));
			
			if (printOutput)
			{
				cout << endl << endl << file->getBoxedString() << endl;
			}
		}
		catch (PineconeError err)
		{
			err.log();
		}
	}
	
	if (!error.getIfErrorLogged())
	{
		try
		{
			lexString(file, tokens);
		}
		catch (PineconeError err)
		{
			err.log();
			astRoot=AstVoid::make();
		}
		
		/*
		if (printOutput)
		{
			cout << endl << tableStringFromTokens(tokens, "lexer output") << endl;
		}
		*/
	}
	
	//astRoot=parseFunction(tokens, 0, tokens.size()-1, Void, Void);
	
	if (!error.getIfErrorLogged())
	{
		try
		{
			astRoot=astNodeFromTokens(tokens, 0, tokens.size()-1);
		}
		catch (PineconeError err)
		{
			err.log();
			astRoot=AstVoid::make();
		}
		
		if (printOutput)
		{
			cout << " ╭──────────────────────╮" << endl;
			cout << " │ abstract syntax tree │" << endl;
			cout << " ╰──────────────────────╯" << endl;
			cout << astRoot->getString() << endl;
			//cout << endl << putStringInBox(astRoot->getString(), "abstract syntax tree") << endl;
			//cout << endl << str::getBoxedString(astRoot->getString(), "abstract syntax tree") << endl;
		}
	}
	
	if (!error.getIfErrorLogged())
	{
		try
		{
			astRoot->setInput(globalNamespace, true, Void, Void);
		}
		catch (PineconeError err)
		{
			err.log();
			astRoot=AstVoid::make();
		}
		
		try
		{
			actionRoot=astRoot->getAction();
			
			if (printOutput)
			{
				cout << " ╭─────────────╮" << endl;
				cout << " │ action tree │" << endl;
				cout << " ╰─────────────╯" << endl;
				cout << actionRoot->getDescription() << endl;
				//cout << endl << str::getBoxedString(actionRoot->getDescription(), "action tree") << endl;
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

string PineconeProgram::getCpp()
{
	try
	{
		CppProgram outProg;
		actionRoot->addToProg(voidAction, voidAction, &outProg);
		return outProg.getCppCode();
	}
 	catch (PineconeError err)
	{
		err.log();
		return "/* no program due to transpiling error */";
	}
}

void PineconeProgram::execute()
{
	try
	{
		stackPtr=globalFramePtr=malloc(globalNamespace->getStackFrame()->getSize());
		free(actionRoot->execute(nullptr, nullptr));
		free(globalFramePtr);
		stackPtr=globalFramePtr=nullptr;
	}
	catch (PineconeError err)
	{
		err.log();
		cout << endl << ">>>>>>    program aborted due to error    <<<<<<" << endl;
	}
}

