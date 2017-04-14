#pragma once

#include "PineconeProgram.h"

#include <string>
using std::string;
using std::to_string;

#include <vector>
using std::vector;

#include <functional>
using std::string;

#include "utils/stringUtils.h"

//	returns if the substring matches the input
//	nothing needed for unicode
//		in: the string to check against
//		pos: the location in 'in' to line the start of the substring up with
//		subStr: the string to compare with
//		returns: if the substring matches the input at the given location
bool substringMatches(const string& in, int pos, const string& subStr);

//	returns the location of a pattern in a string, or -1 if it can't find it
//	no unicode support yet
//		in: the string to search
//		startPos: where to start looking (inclusive)
//		pattern: the single or multi character pattern to search for, wildcards are not yet supported
//		returns: the absolute position (not the offset) of the first instance of the pattern in the input, or -1 if it can't find it
int searchInString(const string& in, const string& pattern, int startPos=0);

//	replaces all instances of one substring with another
//		in: the string to search
//		searchFor: pattern to replace
//		replaceWith: new value
//		returns: in after replace operation
string replaceSubstring(const string& in, const string& searchFor, const string& replaceWith);

//	sets the given array to be the given string sliced up using the given pattern
//		in: the string to slice
//		pattern: the pattern used to chop up the string, will not be included in output
//		out: all the substrings will be appended to this
void sliceStringBy(const string& in, const string& pattern, vector<string>& out);

//	indents a string
//		in: the string to indent (can be multi line)
//		indent: the string to use as the indent
//		returns: the string indented
string indentString(const string& in, string indent="    ", int level=1);

//	returns the original string with the tabs replaced by the correct number of spaces
//		in: the string to convert
//		spaceNum: number of spaces per tab
//		returns: a string with the tabs replaced by spaces
string tabsToSpaces(const string& in, int spaceNum=4);

//	runs tabsToSpaces on an entire array of strings
void tabsToSpaces(vector<string>& in);

//	returns the original string but padded with the given padding string
//		in: the string to pad
//		size: the size of the output string
//		alignment:
//			1: left
//			-1: right
//			0: center
//		pad: the string to use for padding (assumes this string is one char long)
//		leftCap: a string to put on the left size of the input (inside the padding) that will not get chopped
//		rightCap: a string to put on the right size of the input (inside the padding) that will not get chopped
//		returns: the padded string, or the chopped string if the input is shorter then the size
string padString(const string& in, int size, int alignment=1, string pad=" ", string leftCap="", string rightCap="");

//	returns the specific line (starting at 1) in the string it is sent
//		in: the (presumably multiline) string
//		lineNum: the line to return
//		returns: the specific line (without newlines), or an empty string if that line is invalid
string getTextOfLine(const string& in, int lineNum);

//	takes an array of lines and puts a box around it
//		in: an array of strings, each which will be on a new line
//		boxName: the name of the box
//		lineNum: the offset of line numbers, no line nums will be displayed if -1
//		alwaysWidthMax: if to always use the max width
//		maxWidth: the max width of a line, will be truncated if longer
//		returns: a string with a box around the content (must be displayed in monospace obviously)
string lineListToBoxedString(const vector<string>& in, string boxName="", int lineNum=-1, bool alwaysWidthMax=false, int maxWidth=-1);

//	puts the contents of a string into a well formatted
//		in: the input stringz
//		boxName: the name that will appear at the top of the box
//		showLineNums: if to show line numbers
//		alwaysWidthMax: if to always use the max width
//		maxWidth: the maximum width of the contents of the box (with borders it may be a bit wider), if any line is longer it will be chopped
//		returns: the boxed string
string putStringInBox(const string& in, string boxName="", bool showLineNums=false, bool alwaysWidthMax=false, int maxWidth=-1);

//	puts the contents of a string into a table, with tabs being verticle seporators and newlines being horizontle ones
//		in: the input string
//		tableName: the name that will apear at the top of the table
//		returns: a string that looks like a table
string putStringInTable(const string& in, string tableName);

// assembles one section of a tree
string assembleTreeString(const string& root, vector<string>& leaves);

//	converts a double to a string, with always at least one zero before and after the decimal
string doubleToString(double in);

int stringToInt(string in);

double stringToDouble(string in);

//	load entire file and return its contents
//		inName: the path to the file to open
//		debug: if to print status several times to stdout
//		returns: the contents of the file, or an empty string if there is an error
string loadEntireFile(string inName, bool debug=false);

bool writeFile(const string& filename, const string& contents, bool debug=false);

// returns a random character that can be an upper case letter, a lower case letter or a digit
char getRandChar();

// given a hint name and a function to check if a name is unique, returns a unique string
// checker should return true if the given name IS valid
// if alwaysAppendRandom, it will always append a number of random digits
string getUniqueString(string hint, std::function<bool (string)> checker, bool alwaysAppendRandom=false);

//	run a shell command and return the output
//		cmd: the command to run
//		returns: the output of the command
string runCmd(string cmd, bool printOutput=false);

//	get width of the current terminal
//		returns: width of the terminal
int getTermWidth();
