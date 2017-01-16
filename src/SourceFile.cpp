#include "../h/SourceFile.h"
#include "../h/msclStringFuncs.h"

SourceFile::SourceFile(string filenameIn, bool printOutput)
{
	filename=filenameIn;
	contents=loadEntireFile(filenameIn, printOutput);
}

string SourceFile::getBoxedString()
{
	return putStringInBox(contents, filename, true);
}

string SourceFile::getLine(int lineNum)
{
	return getTextOfLine(contents, lineNum);
}
