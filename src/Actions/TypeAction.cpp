#include "../../h/Action.h"
#include "../../h/ErrorHandler.h"

class TypeGetAction: public Action
{
public:
	
	TypeGetAction(Type typeIn):
		Action(Type(new TypeBase(TypeBase::METATYPE, typeIn, "customTypeNameHere")), Void, Void)
	{
		setDescription(typeIn->toString()+" (type)");
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

ActionPtr typeGetAction(Type typeIn)
{
	return ActionPtr(new TypeGetAction(typeIn));
}
