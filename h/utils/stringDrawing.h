#pragma once

#include "stringUtils.h"

namespace str
{

// if max width is -1, it will autodetect
string getBoxedString(const string& in, string boxName="", bool showLineNums=false, bool alwaysWidthMax=false, int maxWidth=-1);

// it is assumed that the data is already padded
void putArrayInTreeNodeBox(vector<string>& data);

string putStringInTreeNodeBox(const string& in);

// note that the input may have newlines in each element
string makeList(vector<string>& data);

string makeRootUpBinaryTree(const string& root, const string& leftBranch, const string& rightBranch, const string& leftLeaf, const string& rightLeaf);

//string makeTreeSection(const string& root, vector<string>& leaves);

}

