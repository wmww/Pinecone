#pragma once

#include "PineconeParser.h"
#include <string>

using std::string;
using std::to_string;

//load entire file and return its contents, returns an empty string if there is an error such as file not found
string loadEntireFile(string inName, bool printOutput=true);

//puts the contents of a string into a well formatted box with character wrapping
string putStringInBox(string in, string boxName="", int maxWidth=80);
