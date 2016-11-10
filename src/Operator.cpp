#include "../h/Operator.h"
#include "../h/ErrorHandler.h"

unordered_map<string, shared_ptr<OperatorData>> OperatorData::operators;
vector<int> OperatorData::precedenceLevels;

Operator opPlus=		OperatorData::create("+", 20, 20, true, true, true);
Operator opMinus=		OperatorData::create("-", 20, 20, true, true, true);
Operator opMultiply=	OperatorData::create("*", 30, 30, true, true, true);
Operator opDivide=		OperatorData::create("/", 30, 30, true, true, true);
Operator opEqual=		OperatorData::create("=", 40, 40, true, true, true);
Operator opGreater=		OperatorData::create(">", 40, 40, true, true, true);
Operator opLess=		OperatorData::create("<", 40, 40, true, true, true);
Operator opColon=		OperatorData::create(":", 50, 10, false, false, false);
Operator opComma=		OperatorData::create(",", 60, 60, true, true, false);
Operator opDot=			OperatorData::create(".", 70, 70, true, true, false);
Operator opIf=			OperatorData::create("?", 80, 80, true, true, false);
Operator opLoop=		OperatorData::create("@", 90, 90, true, true, false);
Operator opOpenPeren=	OperatorData::create("(", 100, 100, false, false, false);
Operator opClosePeren=	OperatorData::create(")", 100, 100, false, false, false);

void getOperators(string text, vector<Operator>& out)
{
	auto i=OperatorData::operators.find(text);
	
	if (i==OperatorData::operators.end())
	{
		error.log("unknown operator '" + text + "'", SOURCE_ERROR);
		return;
	}
	else
	{
		out.push_back(i->second);
		return;
	}
	
	/*for (auto i=OperatorData::operators.begin(); i!=OperatorData::operators.end(); i++)
	{
		if (text==(*i)->getText())
		{
			out.push_back(*i);
			return;
		}
	}
	
	//in the future, as more operators are found the text string will become shorter and shorter
	if (!text.empty())
		error.log("unknown operator '" + text + "'", SOURCE_ERROR);
		*/
}

