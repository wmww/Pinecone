#include "../h/Operator.h"
#include "../h/ErrorHandler.h"

unordered_map<string, shared_ptr<OperatorData>> OperatorData::operators;
vector<int> OperatorData::precedenceLevels;

const Operator opPlus=		OperatorData::create("+", 20, 20, true);
const Operator opMinus=		OperatorData::create("-", 20, 20, true);
const Operator opMultiply=	OperatorData::create("*", 30, 30, true);
const Operator opDivide=	OperatorData::create("/", 30, 30, true);
const Operator opEqual=		OperatorData::create("=", 40, 40, true);
const Operator opGreater=	OperatorData::create(">", 40, 40, true);
const Operator opLess=		OperatorData::create("<", 40, 40, true);
const Operator opColon=		OperatorData::create(":", 15, 50, false);
const Operator opComma=		OperatorData::create(",", 60, 60, false);
const Operator opDot=		OperatorData::create(".", 70, 70, false);
const Operator opIf=		OperatorData::create("?", 80, 80, false);
const Operator opLoop=		OperatorData::create("@", 90, 90, false);
const Operator opOpenPeren=	OperatorData::create("(", 0, 100, false);
const Operator opClosePeren=OperatorData::create(")", 100, 0, false);

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

