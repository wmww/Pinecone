#include "../h/SourceFile.h"
#include "../h/msclStringFuncs.h"

SourceFile::SourceFile(string filenameIn, bool printOutput)
{
	filename=filenameIn;
	contents=loadEntireFile(filenameIn, printOutput)+"\n";
}

string SourceFile::getDirPath()
{
	int i=filename.size();
	
	// this will break on windows
	while (i>=0 && filename[i]!='/')
	{
		i--;
	}
	
	return filename.substr(0, i);
}

string SourceFile::getBoxedString()
{
	return putStringInBox(contents, filename, true);
}

string SourceFile::getLine(int lineNum)
{
	return getTextOfLine(contents, lineNum);
}
