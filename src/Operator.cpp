#include "../h/Operator.h"
#include "../h/ErrorHandler.h"

unordered_map<string, shared_ptr<OperatorData>> OperatorData::operators;
vector<int> OperatorData::precedenceLevels;

const Operator opPlus=		OperatorData::create("+", 20, 21, true);
const Operator opMinus=		OperatorData::create("-", 20, 21, true);
const Operator opMultiply=	OperatorData::create("*", 30, 31, true);
const Operator opDivide=	OperatorData::create("/", 30, 31, true);
const Operator opEqual=		OperatorData::create("=", 40, 41, true);
const Operator opGreater=	OperatorData::create(">", 40, 41, true);
const Operator opLess=		OperatorData::create("<", 40, 41, true);
const Operator opColon=		OperatorData::create(":", 55, 15, false);
const Operator opComma=		OperatorData::create(",", 60, 61, false);
const Operator opDot=		OperatorData::create(".", 70, 71, false);
const Operator opIf=		OperatorData::create("?", 80, 81, false);
const Operator opLoop=		OperatorData::create("@", 90, 91, false);
const Operator opOpenPeren=	OperatorData::create("(", 0, 101, false);
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

