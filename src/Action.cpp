#include "../h/Action.h"
#include "../h/ErrorHandler.h"

DataElem * Action::execute(DataElem * inLeft, DataElem * inRight)
{
	if (inLeft->getType().exactlyEquals(getInLeftType()) && inRight->getType().exactlyEquals(getInRightType()))
	{
		return lambda(inLeft, inRight);		
	}
	else
	{
		error.log(string() + __FUNCTION__ + " sent incorrect types", INTERNAL_ERROR);
		return new VoidData();
	}
}

