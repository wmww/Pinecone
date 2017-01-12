#include "../h/PineconeProgram.h"
#include "../h/AllOperators.h"
#include "../h/StackFrame.h"
#include "../h/Namespace.h"

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
//#define PNCN_Tuple(t1, t2) Type(new TypeData(vector<Type>({PNCN##_##t1, PNCN##_##t2}), ""))
#define PNCN_Tuple(t1, t2) makeTuple(vector<NamedType>({{"a", t1}, {"b", t2}}))

#define LAMBDA_HEADER [](void* leftIn, void* rightIn)->void*

#define retrn out=

#define GET_PTR_VAL(typeIn, varInName) =*((getCppType(typeIn)*)(varInName))

#define DO_INSTANTIATE(typeIn, varOutName, valIn) getCppType(typeIn) varOutName valIn;
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
	INSTANTIATE##_##leftType(leftType, left, GET_PTR_VAL(leftType, leftIn))					\
	INSTANTIATE##_##rightType(rightType, right, GET_PTR_VAL(rightType, rightIn))			\
	INSTANTIATE##_##returnType(returnType, out, ;)											\
	lambdaBody;																				\
	CONCAT(RETURN, returnType)(returnType, out)												\
})																							\

Action voidAction;

shared_ptr<StackFrame> stdLibStackFrame;
Namespace stdLibNamespace;

void addAction(string text, Type returnType, Type leftType, Type rightType, function<void*(void*, void*)> lambda)
{
	stdLibNamespace->addAction(lambdaAction(returnType, lambda, leftType, rightType, text), text);
}

void addAction(Operator op, Type returnType, Type leftType, Type rightType, function<void*(void*, void*)> lambda)
{
	stdLibNamespace->addOperator(lambdaAction(returnType, lambda, leftType, rightType, op->getText()), op);
}

Type IntArray=nullptr;

template<typename T>
T getValFromTuple(void* data, Type type, string name)
{
	OffsetAndType a=type->getSubType(name);
	
	if (!a.type)
		throw PineconeError("tried to get invalid property '"+name+"' from tuple "+type->getString(), INTERNAL_ERROR);
		
	return *((T*)((char*)data+a.offset));
}

template<typename T>
void setValInTuple(void* data, Type type, string name, T val)
{
	OffsetAndType a=type->getSubType(name);
	
	if (!a.type)
		throw PineconeError("tried to set invalid property '"+name+"' from tuple "+type->getString(), INTERNAL_ERROR);
	
	*((T*)((char*)data+a.offset))=val;
}

void populatePineconeStdLib()
{
	Namespace table=stdLibNamespace=NamespaceData::makeRootNamespace();
	stdLibStackFrame=table->getStackFrame();
	
	//this makes a new void action after type constants have been created, if left to the original the Void type may not be set up yet
	voidAction=createNewVoidAction();
	
	
	///constansts
	
	table->addAction(voidAction, "void");
	
	bool trueVal=true;
	table->addAction(constGetAction(&trueVal, Bool, "tru"), "tru");
	
	bool falseVal=false;
	table->addAction(constGetAction(&falseVal, Bool, "fls"), "fls");
	
	
	///operators
	
	// +
	func(ops->plus, Int, Int, Int, retrn left+right);
	func(ops->plus, Dub, Dub, Dub, retrn left+right);
	
	// -
	func(ops->minus, Int, Int, Int, retrn left-right);
	func(ops->minus, Dub, Dub, Dub, retrn left-right);
	
	// *
	func(ops->multiply, Int, Int, Int, retrn left*right);
	func(ops->multiply, Dub, Dub, Dub, retrn left*right);
	
	// /
	func(ops->divide, Int, Int, Int, retrn left/right);
	func(ops->divide, Dub, Dub, Dub, retrn left/right);
	
	// =
	func(ops->equal, Bool, Bool, Bool, retrn left==right);
	func(ops->equal, Bool, Int, Int, retrn left==right);
	func(ops->equal, Bool, Dub, Dub, retrn left==right);
	
	// >
	func(ops->greater, Bool, Bool, Bool,
		retrn !left && right);
	
	func(ops->greater, Bool, Int, Int,
		retrn left>right);
	
	func(ops->greater, Bool, Dub, Dub,
		retrn left>right);
	
	// <
	func(ops->less, Bool, Bool, Bool,
		retrn left && !right);
	
	func(ops->less, Bool, Int, Int,
		retrn left<right);
	
	func(ops->less, Bool, Dub, Dub,
		retrn left<right);
	
	
	///basic types
	
	table->addType(Void, "Void");
	table->addType(Bool, "Bool");
	table->addType(Int, "Int");
	table->addType(Dub, "Dub");
	
	
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
	
	func("printc", Void, Void, Int,
		cout << (char)right);
	
	func("inputInt", Int, Void, Void,
		int val;
		std::cin >> val;
		retrn val;);
	
	
	/// int array
	
	TupleTypeMaker maker;
	maker.add("size", Int);
	maker.add("data", Dub);
	IntArray=maker.get();
	
	table->addType(IntArray, "IntArray");
	
	//func("IntArray", IntArray, Void, Int,
	//	retrn Tuple(right, (double)((int*)malloc(Int->getSize()*right)));
	//);
	
	addAction("IntArray", IntArray, Void, Int, LAMBDA_HEADER
		{
			char* out=(char*)malloc(IntArray->getSize());
			int sizeIn=*(int*)rightIn;
			double data;
			*(int**)(&data)=(int*)malloc(Int->getSize()*sizeIn);
			setValInTuple<int>(out, IntArray, "size", sizeIn);
			setValInTuple<double>(out, IntArray, "data", data);
			return out;
		}
	);
	
	addAction("get", Int, IntArray, Int, LAMBDA_HEADER
		{
			int pos=*(int*)rightIn;
			int* arrayPtr=getValFromTuple<int*>(leftIn, IntArray, "data");
			int val=*(arrayPtr+pos);
			int* out=(int*)malloc(sizeof(int));
			*out=val;
			return out;
		}
	);
	
	addAction("set", Void, IntArray, PNCN_Tuple(Int, Int), LAMBDA_HEADER
		{
			Type rightType=PNCN_Tuple(Int, Int);
			int pos=getValFromTuple<int>(rightIn, rightType, "a");
			int val=getValFromTuple<int>(rightIn, rightType, "b");
			int* arrayPtr=getValFromTuple<int*>(leftIn, IntArray, "data");
			*(arrayPtr+pos)=val;
			return nullptr;
		}
	);
	
	/*addAction(
		"print", Void, Void, Type(new TypeData(vector<Type>({Int, Dub}), "")),
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
		Action
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
	
	//cout << endl << putStringInBox(table->getString(), "standard library") << endl;
}

