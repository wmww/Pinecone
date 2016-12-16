
#include "../h/ErrorHandler.h"

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
		return "internal error";
		break;
		
	case RUNTIME_ERROR:
		return "runtime error";
		break;
		
	default:
		return "unknown priority level";
		break;
	}
}

void ErrorHandler::log(string msg, ErrorPriority priority, Token token)
{
	if (priority==SOURCE_ERROR || priority==INTERNAL_ERROR)
		errorHasBeenLogged=true;
	
	if (token)
		cout << token->getFile() << ":" << token->getLine() << ":" << token->getCharPos() << ": ";
	
	cout << priorityToStr(priority) << ": " << msg << endl;
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
