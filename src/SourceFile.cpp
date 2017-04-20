#include "../h/SourceFile.h"
#include "../h/utils/fileUtils.h"
#include "../h/msclStringFuncs.h"

SourceFile::SourceFile(string filenameIn, bool printOutput)
{
	filename=filenameIn;
	try
	{
		loadFile(filenameIn, contents);
		contents+="\n";
	}
	catch (string err)
	{
		throw PineconeError(err, SOURCE_ERROR);
	}
	
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
	return str::getBoxedString(contents, filename, true);
}

string SourceFile::getLine(int lineNum)
{
	return getTextOfLine(contents, lineNum);
}
