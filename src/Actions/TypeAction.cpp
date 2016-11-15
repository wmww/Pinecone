#include "../../h/Action.h"
#include "../../h/ErrorHandler.h"

class TypeGetAction: public Action
{
public:
	
	TypeGetAction(Type typeIn, string textIn):
		Action(Type(new TypeBase(TypeBase::METATYPE, typeIn, textIn)), Void, Void, textIn)
	{
		if (typeIn->getType()!=TypeBase::METATYPE)
		{
			error.log("TypeGetAction created with type that is not a METATYPE", INTERNAL_ERROR);
		}
		
		setDescription(textIn + " (type)");
	}
	
	string getCSource(string inLeft, string inRight)
	{
		return "/* C source for TypeGetAction not yet implemented */";
	}
	
	void* execute(void* inLeft, void* inRight)
	{
		error.log("TypeGetAction::execute called, which shouldn't happen", RUNTIME_ERROR);
		
		return nullptr;
	}
	
private:
};

ActionPtr typeGetAction(Type typeIn, string textIn)
{
	return ActionPtr(new TypeGetAction(typeIn, textIn));
}
