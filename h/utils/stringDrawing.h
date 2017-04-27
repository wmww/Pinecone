#pragma once

#include "stringUtils.h"

namespace str
{

// if max width is -1, it will autodetect
string getBoxedString(const string& in, string boxName="", bool showLineNums=false, bool alwaysWidthMax=false, int maxWidth=-1);

// it is assumed that the data is already padded
void lightlyBoxStringArray(vector<string>& data);

string getLightlyBoxedString(const string& in);

string makeRootUpBinaryTree(const string& root, const string& leftLeaf, const string& rightLeaf);

//string makeTreeSection(const string& root, vector<string>& leaves);

}

