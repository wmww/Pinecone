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
#include "ActionTable.h"
#include "ElementData.h"
#include "ElementList.h"
#include "FunctionAction.h"
#include "StackFrame.h"

class Element;

void populatePineconeStdLib(ActionTablePtr t);

class PineconeParser
{
public:
	
	PineconeParser();
	~PineconeParser() {cleanUp();}
	
	void setInFileName(std::string in) {inFileName=in;}
	void setSource(std::string in) {inSource=in;}
	string getCpp() {return outCpp;}
	
	void resolveProgram(bool printExtraOutput);
	
	ActionTablePtr getGlobalActionTable();
	
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
	void populateCharVectors();
	void initialProgramPopulation();
	ElementData::Type getElementType(char c, ElementData::Type previousType);
	ElementPtr makeElement(ElementData data);
	
private:
	
	StackFrame globalFrame;
	
	string inFileName;
	string inSource;
	string outCpp;
	
	vector<char> whitespaceChars, letterChars, digitChars, operatorChars;
	char singleLineComment;
};

