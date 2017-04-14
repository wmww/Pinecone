#pragma once

#include "StringUtils.h"

namespace str
{

// if max width is -1, it will autodetect
string getBoxedString(const string& in, string boxName="", bool showLineNums=false, bool alwaysWidthMax=false, int maxWidth=-1);

string makeTreeSection(const string& root, vector<string>& leaves);

}

