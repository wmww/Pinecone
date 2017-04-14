#pragma once

#include "stringUtils.h"

namespace str
{

void splitBy(vector<string>& out, const string& in, const string& splitter, bool keepSplitter=false);

//void splitBy(vector<string>& out, const string& in, vector<string>& splitters, bool keepSplitter=false);

inline void splitByLine(vector<string>& out, const string& in);

int getMaxWidth(vector<string>& in);

// if size is -1 then it is determined as the max width of all the elements in out
void padWidths(vector<string>& out, int size=-1, StringPadAlignment alignment=ALIGNMENT_LEFT, string pad=" ", string leftCap="", string rightCap="");

string join(vector<string>& in, string joiner="\n", bool addAtEnd=true);


/// inline implementations

inline void splitByLine(vector<string>& out, const string& in)
{
	splitBy(out, in, "\n");
}

}

