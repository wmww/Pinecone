#include "../h/SourceFile.h"
#include "../h/msclStringFuncs.h"

SourceFile::SourceFile(string filenameIn)
{
	filename=filenameIn;
	contents=loadEntireFile(filenameIn, true);
}

string SourceFile::getBoxedString()
{
	return putStringInBox(contents, filename, true);
}

string SourceFile::getLine(int lineNum)
{
	return getTextOfLine(contents, lineNum);
}
