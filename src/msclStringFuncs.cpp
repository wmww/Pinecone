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

string replaceSubstring(const string& in, const string& searchFor, const string& replaceWith)
{
	string out;
	int i=0;
	
	while (true)
	{
		int j=searchInString(in, i, searchFor);
		
		if (j<0)
			break;
		else
		{
			out+=in.substr(i, j-i);
			out+=replaceWith;
			i=j+searchFor.size();
		}
	}
	
	out+=in.substr(i, string::npos);
	
	return out;
}

void sliceStringBy(const string& in, const string& pattern, vector<string>& out)
{
	int start=0;
	
	if (pattern.size()<1)
		return;
	
	while (start<int(in.size()))
	{
		int end=searchInString(in, start, pattern);
		
		if (end<0)
			end=in.size();
		
		out.push_back(in.substr(start, end-start));
		
		start=end+pattern.size();
	}
}

string indentString(const string& in, string indent)
{
	string out;
	int start=0;
	
	if (!in.empty())
		out+=indent;
	
	for (int i=0; i<int(in.size()); i++)
	{
		if (in[i]=='\n')
		{
			out+=in.substr(start, i-start+1);
			out+=indent;
			start=i+1;
		}
	}
	
	if (start<=int(in.size()))
		out+=in.substr(start, in.size()-start);
	
	return out;
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

string getTextOfLine(const string& in, int lineNum)
{
	int start=-1;
	int end=-1;
	
	if (lineNum<1)
	{
		return "";
	}
	else if (lineNum==1)
	{
		start=0;
	}
	
	int line=1;
	
	for (unsigned i=0; i<in.size(); i++)
	{
		if (in[i]=='\n')
		{
			if (start<0)
			{
				line++;
				
				if (line==lineNum)
				{
					start=i+1;
				}
			}
			else
			{
				end=i;
				break;
			}
		}
	}
	
	if (start<end)
		return in.substr(start, end-start);
	else if (start>=0)
		return in.substr(start, string::npos);
	else
		return "";
}

string lineListToBoxedString(const vector<string>& in, string boxName, int lineNum, int maxWidth)
{
	string out;
	
	auto first=in.begin();
	auto last=in.end();
	
	if (first!=last)
	{
		last--;
		
		while(first!=last && *first=="")
		{
			first++;
			if (lineNum>=0)
				lineNum++;
		}
		
		while (first!=last && *last=="")
		{
			last--;
		}
		
		last++;
	}
	
	//the extra width of the padding on the right and left (not the virt lines)
	int extraWidth=(lineNum<0)?4:10;
	
	//the size of the contents (not the container)
	int size=boxName.size()-extraWidth+6;
	
	for (auto i: in)
	{
		size=max(size, int(i.size()));
	}
	
	size=min(maxWidth, size);
	
	if (boxName=="")
		out+="  "+padString("", size+extraWidth, 1, "_")+"  ";
	else
		out+="  _"+padString(boxName, size+extraWidth-2, 0, "_", "[ ", " ]")+"_  ";
	
	out+="\n |"+padString("", size+extraWidth, 1, " ")+"| ";
	
	auto i=first;
	
	while (i!=last)
	{
		if (lineNum<0)
		{
			out+="\n |  ";
		}
		else
		{
			out+="\n |"+padString(to_string(lineNum), 4, -1)+"    ";
			lineNum++;
		}
		
		out+=padString(*i, size, 1)+"  | ";
			
		i++;
	}
	
	out+="\n |"+padString("", size+extraWidth, 1, "_")+"| \n";
	
	return out;
}

string putStringInBox(const string& in, string boxName, bool showLineNums, int maxWidth)
{
	vector<string> lines;
	
	sliceStringBy(in, "\n", lines);
	
	tabsToSpaces(lines);
	
	string out=lineListToBoxedString(lines, boxName, showLineNums?1:-1, maxWidth);
	
	return out;
}

string putStringInTable(const string& in, string tableName)
{
	vector<string> lineStrs;
	
	sliceStringBy(in, "\n", lineStrs);
	
	vector<vector<string>> table;
	vector<int> widths;
	//vector<string> lines;
	
	for (auto i: lineStrs)
	{
		vector<string> row;
		
		sliceStringBy(i, "\t", row);
		
		for (unsigned j=0; j<row.size(); j++)
		{
			if (j==widths.size())
				widths.push_back(0);
			
			widths[j]=max(widths[j], int(row[j].size()));
		}
		
		table.push_back(row);
	}
	
	//make sure each row in the table has the same number of cells
	for (auto i: lineStrs)
	{
		for (int j=0; j<int(widths.size())-int(table.back().size()); j++)
			table.back().push_back("");
	}
	
	int totalWidth=0;
	
	for (auto i: widths)
		totalWidth+=i;
	
	string out;
	
	out+="  _"+padString(tableName, totalWidth+2+(table.back().size()-1)*5, 0, "_", "[ ", " ]")+"_  ";
	
	for (auto i: table)
	{
		out+="\n |";
		
		for (unsigned j=0; j<i.size(); j++)
		{
			if (j>0)
				out+="|";
			
			out+=padString("", widths[j]+4, 1, " ");
		}
		
		//out+="  | \n |  "+padString("", totalWidth+(table.back().size()-1)*5, 1, " ")+"  | ";
		
		out+="| ";
		
		out+="\n |  ";
		
		for (unsigned j=0; j<i.size(); j++)
		{
			if (j>0)
				out+="  |  ";
			
			int alignment=(j==i.size()-1)?(1):((j==0)?(-1):0);
			
			out+=padString(i[j], widths[j], alignment);
		}
		
		out+="  | ";
	}
	
	out+="\n |";
	
	for (unsigned j=0; j<table.back().size(); j++)
	{
		if (j>0)
			out+="|";
		
		out+=padString("", widths[j]+4, 1, "_");
	}
	
	out+="| \n";
	
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

//NOTE: I copied this from where I copied this from somewhere on the internet. no idea how or why it works.
string runCmd(string cmd)
{
	const int bufferSize=4096;
    char buffer[bufferSize];
    std::string result = "";
    FILE* pipe = popen(cmd.c_str(), "r");
    if (!pipe) throw std::runtime_error("popen() failed in getOutputFromCmd");
    try {
        while (!feof(pipe)) {
            if (fgets(buffer, bufferSize, pipe) != NULL)
                result += buffer;
        }
    } catch (...) {
        pclose(pipe);
        throw;
    }
    pclose(pipe);
    return result;
}
