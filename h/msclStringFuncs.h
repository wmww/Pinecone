#pragma once

#include "PineconeProgram.h"

#include <string>
using std::string;
using std::to_string;

#include <vector>
using std::vector;

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
int searchInString(const string& in, int startPos, const string& pattern);

//	sets the given array to be the given string sliced up using the given pattern
//		in: the string to slice
//		pattern: the pattern used to chop up the string, will not be included in output
//		out: all the substrings will be appended to this
void sliceStringBy(const string& in, const string& pattern, vector<string>& out);

//	indents a string
//		in: the string to indent (can be multi line)
//		indent: the string to use as the indent
//		returns: the string indented
string indentString(const string& in, string indent="    ");

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

//	takes an array of lines and puts a box around it
//		in: an array of strings, each which will be on a new line
//		boxName: the name of the box
//		maxWidth: the max width of a line, will be truncated if longer
//		lineNum: the offset of line numbers, no line nums will be displayed if -1
//		returns: a string with a box around the content (must be displayed in monospace obviously)
string lineListToBoxedString(const vector<string>& in, string boxName="", int lineNum=-1, int maxWidth=80);

//	puts the contents of a string into a well formatted
//		in: the input string
//		showLineNums: if to show line numbers
//		boxName: the name that will appear at the top of the box
//		maxWidth: the maximum width of the contents of the box (with borders it may be a bit wider), if any line is longer it will be chopped
//		returns: the boxed string
string putStringInBox(const string& in, bool showLineNums=false, string boxName="", int maxWidth=80);

//	puts the contents of a string into a table, with tabs being verticle seporators and newlines being horizontle ones
//		in: the input string
//		tableName: the name that will apear at the top of the table
//		returns: a string that looks like a table
string putStringInTable(const string& in, string tableName);

//	load entire file and return its contents
//		inName: the path to the file to open
//		printOutput: if to print status several times to stdout
//		returns: the contents of the file, or an empty string if there is an error
string loadEntireFile(string inName, bool printOutput=true);

