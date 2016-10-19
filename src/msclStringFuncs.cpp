#include "../h/msclStringFuncs.h"

#include <fstream>
#include <sstream>

using std::stringstream;

string loadEntireFile(string inName, bool printOutput)
{
	std::fstream inFile;
	
	if (printOutput)
		cout << "attempting to open '" << inName << "'..." << endl;
	
	inFile.open(inName);
	
	if (!inFile.is_open())
	{
		if (printOutput)
			cout << "'" << inName << "' failed to open :(" << endl;
		return "";
	}
	else
	{
		if (printOutput)
			cout << "file opended, reading file..." << endl;
		
		stringstream strStream;
		strStream << inFile.rdbuf();//read the file
		string out = strStream.str();//str holds the content of the file
		inFile.close();
		
		if (printOutput)
			cout << "file reading done, '" << inName << "' closed" << endl;
		
		return out;
	}
}

string putStringInBox(string in, bool showLineNums, string boxName, int maxWidth)
{
	const int tabWidth=4;
	int width=0;
	int xPos=0;
	int yStart=0;
	int yPos;
	
	if (!boxName.empty())
		width=boxName.size()+4;
	
	while (in.size()>0 && in[0]=='\n')
	{
		in.erase(in.begin());
		yStart++;
	}
	
	while (in.back()=='\n')
		in.pop_back();
	
	yPos=yStart;
	
	for (auto i=in.begin(); i<=in.end(); ++i)
	{
		if (i==in.end() || *i=='\n')
		{
			width=max(width, xPos);
			xPos=0;
			yPos++;
		}
		else if (*i=='\t')
		{
			xPos+=tabWidth-(xPos%tabWidth);
		}
		else
		{
			xPos++;
		}
	}
	
	width=min(width, maxWidth);
	
	string out;
	
	out+="  __";
	
	if (boxName.empty())
	{
		for (int i=0; i<width; ++i)
			out+="_";
	}
	else
	{
		for (int i=0; i<floor((width-4-boxName.size())/2.0); ++i)
			out+="_";
		
		out+="[ ";
		out+=boxName;
		out+=" ]";
		
		for (int i=0; i<ceil((width-4-boxName.size())/2.0); ++i)
			out+="_";
	}
	
	out+="__  \n |  ";
	
	for (int i=0; i<width; ++i)
		out+=" ";
	
	out+="  | \n |  ";
	
	xPos=0;
	
	for (auto i=in.begin(); i<=in.end(); ++i)
	{
		if (i==in.end() || xPos>=width || *i=='\n')
		{
			for (int j=0; j<width-xPos; ++j)
				out+=" ";
			
			out+="  | \n";
			
			if (i!=in.end())
				out+=" |  ";
			
			xPos=0;
			
			if (i!=in.end() && *i!='\n')
			{
				out+="  -> ";
				xPos+=5;
			}
		}
		
		if (*i=='\t')
		{
			int spaces=tabWidth-(xPos%tabWidth);
			for (int j=0; j<spaces; ++j)
			{
				out+=" ";
				xPos++;
			}
		}
		else if (*i!='\n')
		{
			out+=*i;
			xPos++;
		}
	}
	
	out+=" |__";
	
	for (int i=0; i<width; ++i)
		out+="_";
	
	out+="__| \n";
	
	return out;
}

