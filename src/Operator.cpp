#include "../h/Operator.h"
#include "../h/ErrorHandler.h"

vector<shared_ptr<OperatorBase>> OperatorBase::operators;

Operator opPlus(new OperatorBase("+", true, true, true));
Operator opMinus(new OperatorBase("-", true, true, true));
Operator opColon(new OperatorBase(":", true, true, false));
Operator opDot(new OperatorBase(".", true, true, false));
Operator opOpenPeren(new OperatorBase("(", false, false, false));
Operator opClosePeren(new OperatorBase(")", false, false, false));

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
