#pragma once

#include <string>
using std::string;

class SourceFile
{
public:
	
	SourceFile()
	{
		filename="[empty_file]";
		contents="";
	}
	
	SourceFile(string filenameIn, bool printOutput);
	
	string getFilename() {return filename;}
	
	// get the path to the directory this file is in
	string getDirPath();
	
	const string& getContents() {return contents;}
	
	string getBoxedString();
	
	string getLine(int lineNum);
	
	inline int size()
	{
		return contents.size();
	}
	
	inline char operator[](int i)
	{
		return contents[i];
	}
	
	inline string substr(size_t start, size_t len)
	{
		return contents.substr(start, len);
	}
	
private:
	
	string filename;
	string contents;
};
