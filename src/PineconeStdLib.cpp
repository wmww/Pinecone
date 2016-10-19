#include "../h/PineconeParser.h"

ActionTablePtr table;

#define Dub_CPP double
#define Int_CPP int
#define Bool_CPP bool
#define Void_CPP char

#define Dub_PNCN Type(Type::DUB)
#define Int_PNCN Type(Type::INT)
#define Bool_PNCN Type(Type::BOOL)
#define Void_PNCN Type(Type::VOID)

#define CONC(a,b) a##_##b

#define getPncnType(typeIn) CONC(typeIn, PNCN)
#define getCppType(typeIn) CONC(typeIn, CPP)


#define lambdaHeader [](void* leftIn, void* rightIn)->void*

#define retrn out=

#define func(nameText, returnType, leftType, rightType, lambdaBody)		\
addAction(nameText, getPncnType(returnType), getPncnType(leftType), getPncnType(rightType), lambdaHeader\
{																		\
	getCppType(leftType) left=*((getCppType(leftType)*)leftIn);			\
	getCppType(rightType) right=*((getCppType(rightType)*)rightIn);		\
	getCppType(returnType) out;											\
	lambdaBody;															\
	return getPncnType(returnType).cloneVoidPtr(&out);					\
})																		\

void addAction(Action* in)
{
	table->addAction(ActionPtr(in));
}

void addAction(string text, Type returnType, Type leftType, Type rightType, function<void*(void*, void*)> lambda)
{
	addAction(new LambdaAction(returnType, lambda, leftType, rightType, text));
}

void populatePineconeStdLib(ActionTablePtr t)
{
	table=t;
	
	func("+", Dub, Dub, Dub,
		retrn left+right);
	
	func("+", Int, Int, Int,
		retrn left+right);
	
	func("+", Dub, Dub, Int,
		retrn left+right);
		
	func("print", Void, Void, Bool,
		cout << right);
	
	func("print", Void, Void, Int,
		cout << right);
	
	func("print", Void, Void, Dub,
		cout << right);
	
	
	/*t->addAction
	(
		ActionPtr
		(
			new LambdaAction
			(
				Type(Type::VOID),
				
				[](void* left, void* right)->void*
				{
					cout << *(int*)right << endl;
					return nullptr;
				},
				
				Type(Type::VOID), Type(Type::INT),
				
				"print"
			)
		)
	);*/
	
	//t->addAction()
	
	cout << "standard library:" << endl << table->toString() << endl;
	
	table=nullptr;
}

