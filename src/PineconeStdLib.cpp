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

#define PNCN_String String
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

#define INSTANTIATE_String DO_INSTANTIATE
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

#define func(nameText, leftType, rightType, returnType, lambdaBody, cpp)					\
addAction(nameText, getPncnType(leftType), getPncnType(rightType), getPncnType(returnType), LAMBDA_HEADER\
{																							\
	INSTANTIATE##_##leftType(leftType, left, GET_PTR_VAL(leftType, leftIn))					\
	INSTANTIATE##_##rightType(rightType, right, GET_PTR_VAL(rightType, rightIn))			\
	INSTANTIATE##_##returnType(returnType, out, ;)											\
	lambdaBody;																				\
	CONCAT(RETURN, returnType)(returnType, out)												\
},																							\
cpp																							\
)																							\

Action voidAction;

//shared_ptr<StackFrame> stdLibStackFrame;
Namespace globalNamespace;
Namespace table;

void addAction(string text, Type leftType, Type rightType, Type returnType, function<void*(void*, void*)> lambda, string cpp)
{
	globalNamespace->addAction(lambdaAction(leftType, rightType, returnType, lambda, cpp, text), text);
}

void addAction(Operator op, Type leftType, Type rightType, Type returnType, function<void*(void*, void*)> lambda, string cpp)
{
	globalNamespace->addOperator(lambdaAction(leftType, rightType, returnType, lambda, cpp, op->getText()), op);
}

Type IntArray=nullptr;

template<typename T>
inline T getValFromTuple(void* data, Type type, string name)
{
	OffsetAndType a=type->getSubType(name);
	
	if (!a.type)
		throw PineconeError("tried to get invalid property '"+name+"' from tuple "+type->getString(), INTERNAL_ERROR);
		
	return *((T*)((char*)data+a.offset));
}

template<typename T>
inline void setValInTuple(void* data, Type type, string name, T val)
{
	OffsetAndType a=type->getSubType(name);
	
	if (!a.type)
		throw PineconeError("tried to set invalid property '"+name+"' from tuple "+type->getString(), INTERNAL_ERROR);
	
	*((T*)((char*)data+a.offset))=val;
}

inline string pncnStr2CppStr(void* obj)
{
	int len=getValFromTuple<int>(obj, String, "_size");
	char * data=(char*)malloc((len+1)*sizeof(char));
	memcpy(data, getValFromTuple<char*>(obj, String, "_data"), len*sizeof(char));
	data[len]=0;
	string out(data);
	return out;
}

inline void* cppStr2PncnStr(string cpp)
{
	void * obj=malloc(String->getSize());
	char * strData=(char*)malloc(cpp.size()*sizeof(char));
	memcpy(strData, cpp.c_str(), cpp.size()*sizeof(char));
	
	*((int*)((char*)obj+String->getSubType("_size").offset))=cpp.size();
	*((char**)((char*)obj+String->getSubType("_data").offset))=strData;
	
	return obj;
}

void basicSetup()
{
	table=globalNamespace=NamespaceData::makeRootNamespace();
	
	//this makes a new void action after type constants have been created, if left to the original the Void type may not be set up yet
	voidAction=createNewVoidAction();
}

void populateBasicTypes()
{
	String=makeTuple(vector<NamedType>{NamedType{"_size", Int}, NamedType{"_data", Dub}});
	
	table->addType(Void, "Void");
	table->addType(Bool, "Bool");
	table->addType(Int, "Int");
	table->addType(Dub, "Dub");
	table->addType(String, "String");
}

void populateConstants()
{
	table->addAction(voidAction, "void");
	
	bool trueVal=true;
	table->addAction(constGetAction(&trueVal, Bool, "tru"), "tru");
	
	bool falseVal=false;
	table->addAction(constGetAction(&falseVal, Bool, "fls"), "fls");
	
	// version constant
	{
		Type versionTupleType=
			makeTuple(vector<NamedType>{
				NamedType{"x", Int},
				NamedType{"y", Int},
				NamedType{"z", Int},
			});
		
		void* versionTupleData=malloc(versionTupleType->getSize());
		
		setValInTuple(versionTupleData, versionTupleType, "x", VERSION_X);
		setValInTuple(versionTupleData, versionTupleType, "y", VERSION_Y);
		setValInTuple(versionTupleData, versionTupleType, "z", VERSION_Z);
		
		table->addAction(
			constGetAction(
				versionTupleData,
				versionTupleType,
				"VERSION"
			),
			"VERSION"
		);
	}
}

void populateOperators()
{
	/// ||
	
	func(ops->orOp, Bool, Bool, Bool,
		retrn left || right;
	,
		"$. || $:"
	);
	
	
	/// &&
	
	func(ops->andOp, Bool, Bool, Bool,
		retrn left && right;
	,
		""
	);
	
	
	/// +
	
	func(ops->plus, Int, Int, Int,
		retrn left+right;
	,
		""
	);
	
	func(ops->plus, Dub, Dub, Dub,
		retrn left+right;
	,
		""
	);
	
	
	/// -
	
	func(ops->minus, Int, Int, Int,
		retrn left-right;
	,
		""
	);
	
	func(ops->minus, Dub, Dub, Dub,
		retrn left-right;
	,
		""
	);
	
	
	/// *
	
	func(ops->multiply, Int, Int, Int,
		retrn left*right;
	,
		""
	);
	
	func(ops->multiply, Dub, Dub, Dub,
		retrn left*right;
	,
		""
	);
	
	
	/// /
	
	func(ops->divide, Int, Int, Int,
		retrn left/right;
	,
		""
	);
	
	func(ops->divide, Dub, Dub, Dub,
		retrn left/right;
	,
		""
	);
	
	
	/// %
	
	func(ops->mod, Int, Int, Int,
		retrn left%right;
	,
		""
	);
	
	func(ops->mod, Dub, Dub, Dub,
		retrn left-int(left/right)*right;
	,
		""
	);
	
	/// =
	
	func(ops->equal, Bool, Bool, Bool,
		retrn left==right;
	,
		""
	);
	
	func(ops->equal, Int, Int, Bool,
		retrn left==right;
	,
		""
	);
	
	func(ops->equal, Dub, Dub, Bool,
		retrn left==right;
	,
		""
	);
	
	/// >
	
	func(ops->greater, Int, Int, Bool,
		retrn left>right;
	,
		""
	);
	
	func(ops->greater, Dub, Dub, Bool,
		retrn left>right;
	,
		""
	);
	
	// <
	func(ops->less, Int, Int, Bool,
		retrn left<right;
	,
		""
	);
	
	func(ops->less, Dub, Dub, Bool,
		retrn left<right;
	,
		""
	);
	
	// >=
	func(ops->greaterEq, Int, Int, Bool,
		retrn left>=right;
	,
		""
	);
	
	func(ops->greaterEq, Dub, Dub, Bool,
		retrn left>=right;
	,
		""
	);
	
	// <=
	func(ops->lessEq, Int, Int, Bool,
		retrn left<=right;
	,
		""
	);
	
	func(ops->lessEq, Dub, Dub, Bool,
		retrn left<=right;
	,
		""
	);
	
	// !
	func(ops->notOp, Void, Bool, Bool,
		retrn !right;
	,
		""
	);
	
	func(ops->notOp, Void, Int, Bool,
		retrn right==0;
	,
		""
	);
	
	func(ops->notOp, Void, Dub, Bool,
		retrn right==0;
	,
		""
	);
}

void populateConverters()
{
	///initalizers
	
	func("Bool", Void, Void, Bool,
		retrn false;
	,
		"false"
	);
	
	func("Int", Void, Void, Int,
		retrn 0;
	,
		"0"
	);
	
	func("Dub", Void, Void, Dub,
		retrn 0.0;
	,
		"0.0"
	);
		
	
	///casting
	
	//to bool
	func("Bool", Void, Int, Bool,
		retrn right!=0
	,
		"($: != 0)"
	);
	func("Bool", Void, Dub, Bool,
		retrn right!=0
	,
		"($: != 0.0)"
	);
	
	//to Int
	func("Int", Void, Bool, Int,
		retrn (right?1:0)
	,
		"($: ? 1 : 0)"
	);
	
	func("Int", Void, Dub, Int,
		retrn (int)right
	,
		"((int)$:)"
	);
	
	//to Dub
	func("Dub", Void, Bool, Dub,
		retrn (right?1:0)
	,
		"($: ? 1.0 : 0.0)"
	);
		
	func("Dub", Void, Int, Dub,
		retrn (double)right
	,
		"((double)$:)"
	);
}

void populateStdFuncs()
{
	//print
	
	func("print", Void, Void, Void,
		cout << endl;
	,
		"printf('\n');"
	);
	
	func("print", Void, Bool, Void,
		cout << (right?"tru":"fls") << endl;
	,
		"printf($:?\"tru\":\"fls\");"
	);
	
	func("print", Void, Int, Void,
		cout << right << endl;
	,
		"printf(\"%f\", $:);"
	);
	
	func("print", Void, Dub, Void,
		cout << right << endl;
	,
		"printf(\"%f\", $:);"
	);
	
	addAction("print", Void, String, Void,
		LAMBDA_HEADER
		{
			cout << pncnStr2CppStr(rightIn) << endl;
			return nullptr;
		},
		""
	);
}

void populateStringFuncs()
{
	addAction("String", Void, Void, String,
		LAMBDA_HEADER
		{
			return cppStr2PncnStr("");
		},
		""
	);
	
	addAction("String", Int, Void, String,
		LAMBDA_HEADER
		{
			return cppStr2PncnStr(to_string(*((int*)leftIn)));
		},
		""
	);
	
	addAction("String", Dub, Void, String,
		LAMBDA_HEADER
		{
			return cppStr2PncnStr(to_string(*((double*)leftIn)));
		},
		""
	);
	
	addAction("String", Bool, Void, String,
		LAMBDA_HEADER
		{
			if (*((bool*)leftIn))
			{
				return cppStr2PncnStr("tru");
			}
			else
			{
				return cppStr2PncnStr("fls");
			}
		},
		""
	);
	
	addAction("len", String, Void, Int,
		LAMBDA_HEADER
		{
			int* out=(int*)malloc(sizeof(int));
			*out=getValFromTuple<int>(leftIn, String, "_size");
			return out;
		},
		""
	);
	
	addAction("ascii", Int, Void, String,
		LAMBDA_HEADER
		{
			int val=*((int*)leftIn);
			if (val<0 || val>=256)
			{
				throw PineconeError("tried to make ascii string out of value "+val, RUNTIME_ERROR);
			}
			string out;
			out+=(char)val;
			return cppStr2PncnStr(out);
		},
		""
	);
	
	addAction("at", String, Int, Int,
		LAMBDA_HEADER
		{
			int index=*((int*)rightIn);
			int * out=(int*)malloc(sizeof(int));
			string str=pncnStr2CppStr(leftIn);
			if (index<0 || index>=int(str.size()))
			{
				throw PineconeError("tried to access location "+to_string(index)+" in string "+to_string(str.size())+" long", RUNTIME_ERROR);
			}
			*out=str[index];
			return out;
		},
		""
	);
	
	addAction("sub", String, makeTuple(vector<NamedType>{NamedType{"a", Int}, NamedType{"b", Int}}), String,
		LAMBDA_HEADER
		{
			Type RightType=makeTuple(vector<NamedType>{NamedType{"a", Int}, NamedType{"b", Int}});
			int start=getValFromTuple<int>(rightIn, RightType, "a");
			int end=getValFromTuple<int>(rightIn, RightType, "b");
			string str=pncnStr2CppStr(leftIn);
			if (start<0 || end>int(str.size()) || start>end)
			{
				throw PineconeError("invalid arguments sent to String.sub", RUNTIME_ERROR);
			}
			return cppStr2PncnStr(str.substr(start, end-start));
		},
		""
	);
	
	addAction("input", String, Void, String,
		LAMBDA_HEADER
		{
			string in;
			cout << pncnStr2CppStr(leftIn);
			std::getline (std::cin, in);
			return cppStr2PncnStr(in);
		},
		""
	);
	
	addAction(ops->plus, String, String, String,
		LAMBDA_HEADER
		{
			return cppStr2PncnStr(pncnStr2CppStr(leftIn)+pncnStr2CppStr(rightIn));
		},
		""
	);
	
	addAction(ops->equal, String, String, Bool,
		LAMBDA_HEADER
		{
			bool* out=(bool*)malloc(sizeof(bool));
			*out=pncnStr2CppStr(leftIn)==pncnStr2CppStr(rightIn);
			return out;
		},
		""
	);
}

void populateIntArrayAndFuncs()
{
	TupleTypeMaker maker;
	maker.add("size", Int);
	maker.add("data", Dub);
	IntArray=maker.get();
	
	table->addType(IntArray, "IntArray");
	
	//func("IntArray", IntArray, Void, Int,
	//	retrn Tuple(right, (double)((int*)malloc(Int->getSize()*right)));
	//);
	
	addAction("IntArray", Void, Int, IntArray, LAMBDA_HEADER
		{
			char* out=(char*)malloc(IntArray->getSize());
			int sizeIn=*(int*)rightIn;
			double data;
			int* intPtrData=(int*)malloc(Int->getSize()*sizeIn);
			memcpy(&data, &intPtrData, sizeof(double));
			setValInTuple<int>(out, IntArray, "size", sizeIn);
			setValInTuple<double>(out, IntArray, "data", data);
			return out;
		},
		""
	);
	
	addAction("get", IntArray, Int, Int, LAMBDA_HEADER
		{
			int pos=*(int*)rightIn;
			int size=getValFromTuple<int>(leftIn, IntArray, "size");
			if (pos<0 || pos>=size)
				throw PineconeError("tried to acces position "+to_string(pos)+" of array "+to_string(size)+" long", RUNTIME_ERROR);
			int* arrayPtr=getValFromTuple<int*>(leftIn, IntArray, "data");
			int val=*(arrayPtr+pos);
			int* out=(int*)malloc(sizeof(int));
			*out=val;
			return out;
		},
		""
	);
	
	addAction("set", IntArray, PNCN_Tuple(Int, Int), Void,
		LAMBDA_HEADER
		{
			Type rightType=PNCN_Tuple(Int, Int);
			int pos=getValFromTuple<int>(rightIn, rightType, "a");
			int size=getValFromTuple<int>(leftIn, IntArray, "size");
			if (pos<0 || pos>=size)
				throw PineconeError("tried to set value at position "+to_string(pos)+" of array "+to_string(size)+" long", RUNTIME_ERROR);
			int val=getValFromTuple<int>(rightIn, rightType, "b");
			int* arrayPtr=getValFromTuple<int*>(leftIn, IntArray, "data");
			*(arrayPtr+pos)=val;
			return nullptr;
		},
		""
	);
}

void populateNonStdFuncs()
{
	func("printc", Void, Int, Void,
			cout << (char)right
		,
		""
		);
	
	func("inputc", Void, Void, Int,
			retrn getchar()
		,
		""
		);
	
	func("inputInt", Void, Void, Int,
			int val;
			std::cin >> val;
			retrn val;
		,
		""
		);
	
	addAction("runCmd", Void, String, String,
		LAMBDA_HEADER
		{
			return cppStr2PncnStr(runCmd(pncnStr2CppStr(rightIn)));
		},
		""
	);
}

void populatePineconeStdLib()
{
	basicSetup();
	populateBasicTypes();
	populateConstants();
	populateOperators();
	populateConverters();
	populateStdFuncs();
	populateStringFuncs();
	populateIntArrayAndFuncs();
	populateNonStdFuncs();
}

