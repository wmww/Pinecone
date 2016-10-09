#pragma once

#include "ElementData.h"

#include <string>
using std::string;

enum ErrorPriority
{
	SOURCE_ERROR,
	SOURCE_WARNING,
	JSYK,
	INTERNAL_ERROR
};

class ErrorHandler
{
public:
	
	static string priorityToStr(ErrorPriority in);
	
	void log(string msg, ElementData data, ErrorPriority priority);
	
	void msg(string in);
	
private:
};

extern ErrorHandler error;
