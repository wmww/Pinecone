#include "../h/msclStringFuncs.h"

#include <fstream>
#include <sstream>

using std::stringstream;

bool substringMatches(const string& in, int pos, const string& subStr)
{
	//check if the substring extends past the string size
	if (in.size()<pos+subStr.size())
		return false;
	
	for (unsigned i=0; i<subStr.size(); ++i)
	{
		if (in[i+pos]!=subStr[i])
			return false;
	}
	
	return true;
}

int searchInString(const string& in, int startPos, const string& pattern)
{
	for (unsigned i=startPos; i<in.size(); i++)
	{
		if (substringMatches(in, i, pattern))
			return i;
	}
	
	return -1;
}

void sliceStringBy(const string& in, const string& pattern, vector<string>& out)
{
	unsigned start=0;
	
	if (pattern.size()<1)
		return;
	
	while (start<in.size())
	{
		int end=searchInString(in, start, pattern);
		
		if (end<0)
			end=in.size();
		
		out.push_back(in.substr(start, end-start));
		
		start=end+pattern.size();
	}
}

string tabsToSpaces(const string& in, int spaceNum)
{
	int xPos=0;
	string out;
	int sectionStart=0;
	
	for (unsigned i=0; i<in.size(); i++)
	{
		if (substringMatches(in, i, "\t"))
		{
			string tabStr;
			
			for (int j=0; j<spaceNum-(xPos%spaceNum); j++)
			{
				tabStr+=" ";
			}
			
			out+=in.substr(sectionStart, i-sectionStart)+tabStr;
			
			sectionStart=i+1;
		}
		
		xPos++;
		
		if (substringMatches(in, i, "\n"))
		{
			xPos=0;
		}
	}
	
	out+=in.substr(sectionStart, in.size());
	
	return out;
}

void tabsToSpaces(vector<string>& in)
{
	for (unsigned i=0; i<in.size(); i++)
	{
		in[i]=tabsToSpaces(in[i]);
	}
}

string padString(const string& in, int size, int alignment,  string pad, string leftCap, string rightCap)
{
	int capSize=leftCap.size()+rightCap.size();
	int padSize=size-int(in.size()+capSize);
	
	if (padSize<0) // need to chop
	{
		if (size-capSize>=4)
		{
			return leftCap+in.substr(0, size-capSize-3)+"..."+rightCap;
		}
		else if (size-capSize>=0)
		{
			return leftCap+string("...").substr(0, size-capSize)+rightCap;
		}
		else
		{
			return (leftCap+rightCap).substr(0, size);
		}
	}
	else if (padSize==0) // input size was exactly right
	{
		return leftCap+in+rightCap;
	}
	else // need to pad
	{
		if (alignment==0) // center alignment
		{
			string leftPad, rightPad;
			
			for (int i=0; i<floor(padSize/2.0); i++)
				leftPad+=pad;
			
			for (int i=0; i<ceil(padSize/2.0); i++)
				rightPad+=pad;
			
			return leftPad+leftCap+in+rightCap+rightPad;
		}
		// left or right alignment
		else
		{
			string padStr;
			
			for (int i=0; i<padSize; i++)
				padStr+=pad;
			
			if (alignment>0) // right align
				return leftCap+in+rightCap+padStr;
			else // left align
				return padStr+leftCap+in+rightCap;
		}

	}
}

string lineListToBoxedString(const vector<string>& in, string boxName, int lineNum, int maxWidth)
{
	string out;
	
	boxName=boxName;
	
	int maxSize=boxName.size()+2;
	
	for (auto i: in)
	{
		maxSize=max(maxSize, int(i.size()));
	}
	
	maxSize=min(maxWidth, maxSize);
	
	out+="  _"+padString(boxName, maxSize+2, 0, "_", "[ ", " ]")+"_  ";
	
	for (auto i: in)
	{
		out+="\n |  "+padString(i, maxSize, 1)+"  | ";
	}
	
	out+="\n |__"+padString("", maxSize, 1, "_")+"__| ";
	
	return out;
}

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
	vector<string> lines;
	
	sliceStringBy(in, "\n", lines);
	
	tabsToSpaces(lines);
	
	string out=lineListToBoxedString(lines, boxName, showLineNums?0:-1);
	
	return out;
}

string putStringInTable(string in, bool showLineNums, bool showOuterLines, bool showInnerLines, string tableName)
{
	vector<string> lineStrs;
	
	sliceStringBy(in, "\n", lineStrs);
	
	vector<vector<string>> table;
	vector<int> widths;
	vector<string> lines;
	
	for (auto i: lineStrs)
	{
		table.push_back(vector<string>());
		
		sliceStringBy(i, "\t", table.back());
		
		for (unsigned j=0; j<table.back().size(); j++)
		{
			if (j>=widths.size())
				widths.push_back(0);
			
			widths[j]=max(widths[j], int(table.back()[j].size()));
		}
		
		for (int j=0; j<int(widths.size())-int(table.back().size()); j++)
			table.back().push_back("");
	}
	
	for (auto i: table)
	{
		lines.push_back("");
		
		for (unsigned j=0; j<i.size(); j++)
		{
			lines.back()+=padString(i[j], widths[j]);
			if (j<i.size()-1)
				lines.back()+="  |  ";
		}
	}
	
	string out=lineListToBoxedString(lines, tableName);
	
	return out;
	
	/*int colPos=0;
	int colWidth=0;
	char outerVirtLine=showOuterLines?'|':' ';
	
	vector<int> cols({0});
	
	for (unsigned i=0; i<in.size(); i++)
	{
		if (in[i]=='\t')
		{
			colPos++;
			while (cols.size()<(unsigned)colPos+1)
				cols.push_back(0);
			colWidth=0;
		}
		else if (in[i]=='\n')
		{
			colPos=0;
			colWidth=0;
		}
		else
		{
			colWidth++;
			cols[colPos]=max(cols[colPos], colWidth);
		}
	}
	
	colPos=0;
	colWidth=0;
	
	int totalWidth=0;
	
	for (unsigned i=0; i<cols.size(); i++)
		totalWidth+=cols[i];
	
	string out;
	
	//out+=getBoxNameLine(tableName, showOuterLines, 12);
	
	out=out + " " + outerVirtLine + "  ";
	
	for (int i=0; i<totalWidth; ++i)
		out+=" ";
	
	out=out + "  " + outerVirtLine + " \n " + outerVirtLine + "  ";
	
	for (unsigned i=0; i<in.size(); i++)
	{
		if (in[i]=='\t' || in[i]=='\n')
		{
			for (int j=colWidth; j<cols[colPos]; j++)
				out+=" ";
		}
		
		if (in[i]=='\t')
		{
			if (showInnerLines)
				out+="|";
			else
				out+=" ";
			
			colPos++;
			colWidth=0;
		}
		else if (in[i]=='\n')
		{
			colPos=0;
			colWidth=0;
			out=out + outerVirtLine + "\n " + outerVirtLine;
		}
		else
		{
			colWidth++;
			out+=in[i];
		}
	}
	
	out+="\n |__";
	
	for (int i=0; i<totalWidth; ++i)
		out+="_";
	
	out+="__| \n";
	
	return out;*/
}

string getBoxNameLine(string name, bool showLine, int width)
{
	if (name=="" && !showLine)
		return "";
	
	string out;
	
	char lineChar=showLine?'_':' ';
	
	out+="  ";
	out+=lineChar;
	out+=lineChar;
	
	if (name.empty())
	{
		for (int i=0; i<width; ++i)
			out+=lineChar;
	}
	else
	{
		for (int i=0; i<floor((width-4-(int)name.size())/2.0); i++)
			out+=lineChar;
		
		out+="[ ";
		out+=name;
		out+=" ]";
		
		for (int i=0; i<ceil((width-4-(int)name.size())/2.0); ++i)
			out+=lineChar;
	}
	
	out+=lineChar;
	out+=lineChar;
	out+="  \n";
	
	return out;
}
