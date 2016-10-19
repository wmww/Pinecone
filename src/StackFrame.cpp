#include "../h/StackFrame.h"
#include "../h/msclStringFuncs.h"
#include "../h/ErrorHandler.h"

void StackFrame::clear()
{
	elements.clear();
	actions=nullptr;
	members.clear();
}

void StackFrame::appendElement(ElementPtr in)
{
	elements.appendElement(in);
}

void StackFrame::resolve(bool printOutput)
{
	if (printOutput)
		cout << endl << elements.printToBoxedString("unstructured element list") << endl;
	
	elements.structureByOperators();
	
	if (printOutput)
		cout << endl << elements.printToBoxedString("structured by operators") << endl;
	
	actions=elements.resolveActions();
	
	cout << "printing action tree..." << endl;
	
	if (printOutput)
	{
		if (actions)
			cout << endl << putStringInBox(actions->getDescription(), false, "action tree") << endl;
		else
			cout << endl << "actins is null" << endl;
	}
}

void StackFrame::execute()
{
	if (actions)
		actions->getReturnType().deleteVoidPtr(actions->execute(nullptr, nullptr));
	else
		error.log("cannot execute stack frame because actions is null", RUNTIME_ERROR);
}
