
#include "../../h/utils/fileUtils.h"

#include <fstream>
#include <sstream>

using std::stringstream;

void loadFile(string filepath, string& contents)
{
	std::fstream inFile;
	
	inFile.open(filepath);
	
	if (!inFile.is_open())
	{
		throw "could not open '"+filepath+"'";
	}
	else
	{
		stringstream strStream;
		strStream << inFile.rdbuf();//read the file
		contents = strStream.str();//str holds the content of the file
		inFile.close();
	}
}

void writeFile(string filepath, const string& contents)
{
	std::ofstream outFile;
	
	outFile.open(filepath);
	
	if (!outFile.is_open())
	{
		throw "error writing to '"+filepath+"'";
	}
	else
	{
		outFile << contents;
		
		outFile.close();
	}
}

