#include "../h/SourceFile.h"
#include "../h/msclStringFuncs.h"

SourceFile::SourceFile(string filenameIn)
{
	filename=filenameIn;
	contents=loadEntireFile(filenameIn, false);
}

string SourceFile::getBoxedString()
{
	return putStringInBox(contents, filename, true);
}
