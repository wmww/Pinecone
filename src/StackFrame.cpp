#include "../h/StackFrame.h"
#include "../h/msclStringFuncs.h"

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
	
	elements.resolveActions();
	
	if (printOutput)
		cout << endl << elements.printToBoxedString("identifiers resolved") << endl;
}

void StackFrame::execute()
{
	actions->getReturnType().deleteVoidPtr(actions->execute(nullptr, nullptr));
}
