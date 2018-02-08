
#include "../h/ErrorHandler.h"
#include "../h/msclStringFuncs.h"
#include "../h/SourceFile.h"

#include <iostream>

using std::cout;
using std::endl;

ErrorHandler error;

string ErrorHandler::priorityToStr(ErrorPriority in)
{
	switch (in)
	{
	case SOURCE_ERROR:
		return "error";
		break;
		
	case SOURCE_WARNING:
		return "warning";
		break;
		
	case JSYK:
		return "jsyk";
		break;
		
	case INTERNAL_ERROR:
		return "INTERNAL ERROR";
		break;
		
	case RUNTIME_ERROR:
		return "runtime error";
		break;
		
	default:
		return "UNKNOWN PRIORITY LEVEL";
		break;
	}
}

void ErrorHandler::log(string msg, ErrorPriority priority, Token token)
{
	if (priority==SOURCE_ERROR || priority==INTERNAL_ERROR || priority==RUNTIME_ERROR)
		errorHasBeenLogged=true;
	
	// gcc style
	//if (token)
	//	cout << token->getFile() << ":" << token->getLine() << ":" << token->getCharPos() << ": ";
	
	//cout << priorityToStr(priority) << ": " << msg << endl;
	
	// Pinecone style
	
	cout << priorityToStr(priority);
	
	if (token)
	{
		cout << " in '" << token->getFile()->getFilename() << "' on line " << token->getLine() << ":" << endl;
		cout << indentString(msg, "    ") << endl;
		
		string line=token->getFile()->getLine(token->getLine());
		
		int wspace=0;
		for (; wspace<int(line.size()) && (line[wspace]==' ' || line[wspace]=='\t' || line[wspace]=='\n'); wspace++) {}
		
		string arrows="";
		for (int i=0; i<token->getCharPos()-1-wspace; i++)
			arrows+=" ";
		for (int i=0; i<int(token->getText().size()); i++)
			arrows+="^";
		
		cout << indentString(""+line.substr(wspace, string::npos)+"\n"+arrows, "    ") << endl;
	}
	else
	{
		cout << ": " << msg << endl;
	}
}

void ErrorHandler::msg(string in)
{
	cout << "message: " << in << endl;
}

PineconeError::PineconeError(string msgIn, ErrorPriority priorityIn, Token tokenIn)
{
	msg=msgIn;
	priority=priorityIn;
	token=tokenIn;
}

void PineconeError::log()
{
	error.log(msg, priority, token);
}
