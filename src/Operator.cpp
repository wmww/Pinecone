#include "../h/Operator.h"
#include "../h/ErrorHandler.h"

vector<shared_ptr<OperatorBase>> OperatorBase::operators;

Operator opPlus=OperatorBase::create("+", true, true, true);
Operator opMinus=OperatorBase::create("-", true, true, true);
Operator opEqual=OperatorBase::create("=", true, true, false);
Operator opColon=OperatorBase::create(":", true, true, false);
Operator opDot=OperatorBase::create(".", true, true, false);
Operator opIf=OperatorBase::create("?", true, true, false);
Operator opOpenPeren=OperatorBase::create("(", false, false, false);
Operator opClosePeren=OperatorBase::create(")", false, false, false);

void getOperators(vector<Operator>& out, string text, ElementData data)
{
	for (auto i=OperatorBase::operators.begin(); i!=OperatorBase::operators.end(); i++)
	{
		if (text==(*i)->getText())
		{
			out.push_back(*i);
			return;
		}
	}
	
	//in the future, as more operators are found the text string will become shorter and shorter
	if (!text.empty())
		error.log("unknown operator '" + text + "'", data, SOURCE_ERROR);
}
