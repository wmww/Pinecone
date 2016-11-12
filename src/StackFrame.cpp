#include "../h/StackFrame.h"
#include "../h/msclStringFuncs.h"
#include "../h/ErrorHandler.h"

unsigned char * stackPtr=nullptr;

void StackFrame::clear()
{
	actions=nullptr;
	members.clear();
}

void StackFrame::execute()
{
	if (actions)
		free(actions->execute(nullptr, nullptr));
	else
		error.log("cannot execute stack frame because actions is null", RUNTIME_ERROR);
}
