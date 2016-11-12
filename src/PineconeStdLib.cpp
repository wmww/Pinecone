#include "../h/PineconeProgram.h"
#include "../h/Operator.h"
#include "../h/StackFrame.h"
#include "../h/ActionTable.h"

#define CONCAT(a,b) a##_##b
#define GET_TYPES_Tuple(t0, t1) t0, t1

#define getPncnType(typeIn) CONCAT(PNCN, typeIn)
#define getCppType(typeIn) CONCAT(CPP, typeIn)

#define CPP_Dub double
#define CPP_Int int
#define CPP_Bool bool
#define CPP_Void char

#define PNCN_Dub Dub
#define PNCN_Int Int
#define PNCN_Bool Bool
#define PNCN_Void Void
#define PNCN_Tuple(t1, t2) Type(new TypeBase(vector<Type>({PNCN##_##t1, PNCN##_##t2}), ""))

#define LAMBDA_HEADER [](void* leftIn, void* rightIn)->void*

#define retrn out=

#define GET_PTR_VAL(typeIn, varInName) =*((getCppType(typeIn)*)(varInName))

#define DO_INSTANTIATE(typeIn, varOutName, valIn) getCppType(typeIn) varOutName GET_PTR_VAL(typeIn, valIn);
#define DONT_INSTANTIATE(typeIn, varOutName, valIn) ;
#define INSTANTIATE_CPP_TUPLE(t0, t1, varOutName, valIn)\
	DO_INSTANTIATE(t0, CONCAT(varOutName, 0), valIn)\
	DO_INSTANTIATE(t1, CONCAT(varOutName, 1), ((char *)valIn)+sizeof(getCppType(t0)))\

#define DO_RETURN_VAL(typeIn, varName) void* outPtr=malloc(getPncnType(typeIn)->getSize()); memcpy(outPtr, &varName, getPncnType(typeIn)->getSize()); return outPtr;
#define DONT_RETURN_VAL(typeIn, varName) return nullptr;

#define INSTANTIATE_Dub DO_INSTANTIATE
#define INSTANTIATE_Int DO_INSTANTIATE
#define INSTANTIATE_Bool DO_INSTANTIATE
#define INSTANTIATE_Void DONT_INSTANTIATE
#define INSTANTIATE_Tuple__(typeIn, varOutName, valIn) INSTANTIATE_CPP_TUPLE(typeIn, varOutName, valIn)
#define INSTANTIATE_Tuple_(typeIn, varOutName, valIn) INSTANTIATE_Tuple__(GET_TYPES##_##typeIn, varOutName, valIn)
#define INSTANTIATE_Tuple(t1, t2) INSTANTIATE_Tuple_

#define RETURN_Dub DO_RETURN_VAL
#define RETURN_Int DO_RETURN_VAL
#define RETURN_Bool DO_RETURN_VAL
#define RETURN_Void DONT_RETURN_VAL

#define func(nameText, returnType, leftType, rightType, lambdaBody)							\
addAction(nameText, getPncnType(returnType), getPncnType(leftType), getPncnType(rightType), LAMBDA_HEADER\
{																							\
	INSTANTIATE##_##leftType(leftType, left, leftIn)									\
	INSTANTIATE##_##rightType(rightType, right, rightIn)								\
	INSTANTIATE##_##returnType(returnType, out, nullptr)								\
	lambdaBody;																				\
	CONCAT(RETURN, returnType)(returnType, out)												\
})																							\

ActionPtr voidAction;

StackFrame stdLibStackFrame;
ActionTablePtr stdLibActionTable(new ActionTable(&stdLibStackFrame));

void addAction(string text, Type returnType, Type leftType, Type rightType, function<void*(void*, void*)> lambda)
{
	stdLibActionTable->addAction(lambdaAction(returnType, lambda, leftType, rightType, text));
}

void addAction(Operator op, Type returnType, Type leftType, Type rightType, function<void*(void*, void*)> lambda)
{
	stdLibActionTable->addAction(lambdaAction(returnType, lambda, leftType, rightType, op->getText()), op);
}

void populatePineconeStdLib(ActionTablePtr t)
{
	ActionTablePtr table=stdLibActionTable;
	
	table->clear();
	stdLibStackFrame.clear();
	
	//this makes a new void action after type constants have been created, if left to the original the Void type may not be set up yet
	voidAction=createNewVoidAction();
	
	
	///constansts
	
	table->addAction(voidAction);
	
	bool trueVal=true;
	table->addAction(constGetAction(&trueVal, Bool, "tru"));
	
	bool falseVal=false;
	table->addAction(constGetAction(&falseVal, Bool, "fls"));
	
	
	///operators
	
	// +
	func(opPlus, Int, Int, Int, retrn left+right);
	func(opPlus, Dub, Dub, Dub, retrn left+right);
	
	// -
	func(opMinus, Int, Int, Int, retrn left-right);
	func(opMinus, Dub, Dub, Dub, retrn left-right);
	
	// *
	func(opMultiply, Int, Int, Int, retrn left*right);
	func(opMultiply, Dub, Dub, Dub, retrn left*right);
	
	// /
	func(opDivide, Int, Int, Int, retrn left/right);
	func(opDivide, Dub, Dub, Dub, retrn left/right);
	
	// =
	func(opEqual, Bool, Bool, Bool, retrn left==right);
	func(opEqual, Bool, Int, Int, retrn left==right);
	func(opEqual, Bool, Dub, Dub, retrn left==right);
	
	// >
	func(opGreater, Bool, Bool, Bool,
		retrn !left && right);
	
	func(opGreater, Bool, Int, Int,
		retrn left>right);
	
	func(opGreater, Bool, Dub, Dub,
		retrn left>right);
	
	// <
	func(opLess, Bool, Bool, Bool,
		retrn left && !right);
	
	func(opLess, Bool, Int, Int,
		retrn left<right);
	
	func(opLess, Bool, Dub, Dub,
		retrn left<right);
	
	
	///basic types
	
	table->addType(Void);
	table->addType(Bool);
	table->addType(Int);
	table->addType(Dub);
	
	
	///initalizers
	
	func("Bool", Bool, Void, Void, 
		retrn false);
	
	func("Int", Int, Void, Void, 
		retrn 0);
	
	func("Dub", Dub, Void, Void, 
		retrn 0.0);
		
	
	///casting
	
	//to bool
	func("Bool", Bool, Void, Int, retrn right!=0);
	func("Bool", Bool, Void, Dub, retrn right!=0);
	
	//to Int
	func("Int", Int, Void, Bool, retrn (right?1:0));
	func("Int", Int, Void, Dub, retrn (int)right);
	
	//to Dub
	func("Dub", Dub, Void, Bool, retrn (right?1:0));
	func("Dub", Dub, Void, Int, retrn (double)right);
	
	
	///mscl functions
	
	//print
	
	func("print", Void, Void, Void,
		cout << endl);
	
	func("print", Void, Void, Bool,
		cout << (right?"tru":"fls") << endl);
	
	func("print", Void, Void, Int,
		cout << right << endl);
	
	func("print", Void, Void, Dub,
		cout << right << endl);
	
	/*addAction(
		"print", Void, Void, Type(new TypeBase(vector<Type>({Int, Dub}), "")),
		LAMBDA_HEADER
		{
			cout << "(" << *((int*)rightIn) << ", " << *((double*)((int*)rightIn+1)) << ")" << endl;
			return nullptr;
		}
	);*/
	
	//func("print", Void, Void, Tuple(Int, Dub),
	//	cout<< "(" << right_0 << ", " << right_1 << ")" << endl);
		//cout << "(" << *((int*)rightIn) << ", " << *((double*)((int*)rightIn+1)) << ")" << endl);
	
	
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
	
	cout << endl << putStringInBox(table->toString(), false, "standard library") << endl;
}

