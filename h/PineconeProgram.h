#pragma once

#include <string>
#include <iostream>
#include <math.h>
#include <vector>
#include <list>

using std::max;
using std::min;
using std::cout;
using std::endl;
using std::string;
using std::to_string;
using std::vector;
using std::list;

#include "msclStringFuncs.h"
#include "Namespace.h"
#include "StackFrame.h"
#include "Token.h"
#include "AstNode.h"

class Element;

class PineconeProgram
{
public:
	
	PineconeProgram();
	~PineconeProgram() {cleanUp();}
	
	void setInFileName(std::string in) {inFileName=in;}
	void setSource(std::string in) {inSource=in;}
	string getCpp() {return outCpp;}
	
	void resolveProgram(bool printExtraOutput);
	
	Namespace getGlobalActionTable();
	
	void execute();
	
	string resolveProgram(string input, string fileIn, bool printExtraOutput=true)
	{
		setSource(input);
		setInFileName(fileIn);
		resolveProgram(printExtraOutput);
		return getCpp();
	}
	
private:
	
	void cleanUp();
	
private:
	
	StackFrame globalFrame;
	
	string inFileName;
	string inSource;
	string outCpp;
	
	//a list of all the tokens in the program
	vector<Token> tokens;
	
	//root of the abstract syntax tree
	AstNode astRoot=nullptr;
	
	//root of the action tree
	Action actionRoot=createNewVoidAction();
	
	vector<char> whitespaceChars, letterChars, digitChars;
	char singleLineComment;
};

