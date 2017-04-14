#pragma once

#include <vector>
using std::vector;

#include <unordered_map>
using std::unordered_map;

#include "Operator.h"

class AllOperators
{
public:
	
	// sets ops to a new instance
	static void init();
	
	// sets out to the operators (0, 1 or more) for the given text
	void get(string text, vector<Operator>& out);
	
	/*
	#define DECLARE_OP(name, text, left, right, overload)\
		const Operator name{new OperatorData(text, left, right, overload)};
	
	#define ALL_OPS											\
	DECLARE_OP(	loop,		"@",	5,		5,		false	);	\
	DECLARE_OP(	ifOp,		"?",	5,		5,		false	);	\
	DECLARE_OP(	pipe,		"|",	10,		10,		false	);	\
	DECLARE_OP(	plus,		"+",	20,		21,		true	);	\
	DECLARE_OP(	minus,		"-",	20,		21,		true	);	\
	DECLARE_OP(	multiply,	"*",	30,		31,		true	);	\
	DECLARE_OP(	divide,		"/",	30,		31,		true	);	\
	DECLARE_OP(	equal,		"=",	40,		41,		true	);	\
	DECLARE_OP(	greater,	">",	40,		41,		true	);	\
	DECLARE_OP(	less,		"<",	40,		41,		true	);	\
	DECLARE_OP(	comma,		",",	60,		61,		false	);	\
	DECLARE_OP(	dot,		".",	70,		71,		false	);	\
	DECLARE_OP(	doubleColon,"::",	13,		12,		false	);	\
	DECLARE_OP(	colon,		":",	15,		14,		false	);	\
	DECLARE_OP(	openPeren,	"(",	0,		100,	false	);	\
	DECLARE_OP(	closePeren,	")",	100,	0,		false	);	\
	DECLARE_OP(	openSqBrac,	"[",	0,		100,	false	);	\
	DECLARE_OP(	closeSqBrac,"]",	100,	0,		false	);	\
	DECLARE_OP(	openCrBrac,	"{",	0,		100,	false	);	\
	DECLARE_OP(	closeCrBrac,"}",	100,	0,		false	);	\
	*/
	
	#define DECLARE_OP(name, text, prece, input, overload)\
		const Operator name{new OperatorData(text, prece, input, overload)};
	
	#define ALL_OPS																\
	DECLARE_OP(	loop,		"@",	5,		OperatorData::BOTH,		false	);	\
	DECLARE_OP(	ifOp,		"?",	6,		OperatorData::BOTH,		false	);	\
	DECLARE_OP(	pipe,		"|",	6,		OperatorData::BOTH,		false	);	\
	DECLARE_OP(	colon,		":",	24,		OperatorData::BOTH,		false	);	\
	DECLARE_OP(	doubleColon,"::",	24,		OperatorData::BOTH,		false	);	\
	DECLARE_OP(	comma,		",",	35,		OperatorData::BOTH,		false	);	\
	DECLARE_OP(	orOp,		"||",	36,		OperatorData::BOTH,		false	);	\
	DECLARE_OP(	andOp,		"&&",	38,		OperatorData::BOTH,		false	);	\
	DECLARE_OP(	equal,		"=",	40,		OperatorData::BOTH,		true	);	\
	DECLARE_OP(	notEqual,	"!=",	40,		OperatorData::BOTH,		false	);	\
	DECLARE_OP(	greater,	">",	50,		OperatorData::BOTH,		true	);	\
	DECLARE_OP(	less,		"<",	50,		OperatorData::BOTH,		true	);	\
	DECLARE_OP(	greaterEq,	">=",	50,		OperatorData::BOTH,		true	);	\
	DECLARE_OP(	lessEq,		"<=",	50,		OperatorData::BOTH,		true	);	\
	DECLARE_OP(	plus,		"+",	61,		OperatorData::BOTH,		true	);	\
	DECLARE_OP(	minus,		"-",	61,		OperatorData::BOTH,		true	);	\
	DECLARE_OP(	multiply,	"*",	71,		OperatorData::BOTH,		true	);	\
	DECLARE_OP(	divide,		"/",	71,		OperatorData::BOTH,		true	);	\
	DECLARE_OP(	mod,		"%",	70,		OperatorData::BOTH,		true	);	\
	DECLARE_OP(	notOp,		"!",	74,		OperatorData::RIGHT,	true	);	\
	DECLARE_OP(	plusPlus,	"++",	75,		OperatorData::LEFT,		false	);	\
	DECLARE_OP(	minusMinus,	"--",	75,		OperatorData::LEFT,		false	);	\
	DECLARE_OP(	dot,		".",	81,		OperatorData::BOTH,		false	);	\
	DECLARE_OP(	rightArrow,	"->",	83,		OperatorData::BOTH,		false	);	\
	DECLARE_OP(	import,		"==>",	90,		OperatorData::RIGHT,	false	);	\
	DECLARE_OP(	openPeren,	"(",	100,	OperatorData::RIGHT,	false	);	\
	DECLARE_OP(	closePeren,	")",	99,		OperatorData::LEFT,		false	);	\
	DECLARE_OP(	openSqBrac,	"[",	100,	OperatorData::BOTH,		false	);	\
	DECLARE_OP(	closeSqBrac,"]",	99,		OperatorData::LEFT,		false	);	\
	DECLARE_OP(	openCrBrac,	"{",	100,	OperatorData::RIGHT,	false	);	\
	DECLARE_OP(	closeCrBrac,"}",	99,		OperatorData::LEFT,		false	);	\
	
	ALL_OPS;
	
	// for read only, dont screw with this
	unordered_map<string, Operator>& getOpsMap() {return opsMap;}
	
	// returns if the given operator is an opening bracket or peren
	bool isOpenBrac(Operator op);
	bool isCloseBrac(Operator op);
	
private:
	
	// is only called in init
	AllOperators();
	
	// used internally by the constructor
	void putOpInMap(Operator op);
	
	// a hash map of all the operators
	unordered_map<string, Operator> opsMap;
};

// the single instance of this class, starts out as nullptr but chages to a real instance in AllOperators::init
//extern shared_ptr<AllOperators> ops;
extern AllOperators* ops;
