
#include "../../h/utils/StringDrawing.h"
#include "../../h/utils/StringArray.h"

#include "../../h/msclStringFuncs.h"

namespace str
{

string getBoxedString(const string& in, string boxName, bool showLineNums, bool alwaysWidthMax, int maxWidth)
{
	vector<string> lines;
	int width;
	
	if (maxWidth<0)
	{
		maxWidth=getTermWidth()-4;
	}
	
	str::splitByLine(lines, tabsToSpaces(in));
	
	int startLineNum=1;
	
	while (!lines.empty() && lines[0].empty())
	{
		lines.erase(lines.begin());
		startLineNum++;
	}
	
	while (!lines.empty() && lines.back().empty())
	{
		lines.pop_back();
	}
	
	int nameWidth=getWidth(boxName);
	
	if (showLineNums)
	{
		int numChars=getWidth(to_string(lines.size()+startLineNum-1));
		
		for (int i=0; i<(int)lines.size(); i++)
		{
			lines[i]=pad(to_string(i+startLineNum), numChars+5, ALIGNMENT_RIGHT, " ", "", "    ")+lines[i];
		}
	}
	
	if (alwaysWidthMax)
	{
		width=maxWidth;
	}
	else
	{
		width=std::min(std::max(str::getMaxWidth(lines), nameWidth+4-2), maxWidth);
	}
	
	str::padWidths(lines, width);
	
	for (int i=0; i<(int)lines.size(); i++)
	{
		lines[i]=" ┃ "+lines[i]+" ┃ ";
	}
	
	string underline;
	for (int i=0; i<nameWidth; i++)
	{
		underline+="─";
	}
	
	lines.insert(lines.begin(), " ┃"+pad(underline,	width+2, ALIGNMENT_CENTER, " ", "╰─", "─╯")+"┃ ");
	lines.insert(lines.begin(), " ┏"+pad(boxName,	width+2, ALIGNMENT_CENTER, "━", "┥ ", " ┝")+"┓ ");
	lines.insert(lines.begin(), "  "+pad(underline,	width+2, ALIGNMENT_CENTER, " ", "╭─", "─╮")+"  ");
	
	string bottom;
	string emptyLn;
	
	for (int i=0; i<width; i++)
	{
		bottom+="━";
		emptyLn+=" ";
	}
	
	lines.push_back(" ┃ "+emptyLn+" ┃ ");
	lines.push_back(" ┗━"+bottom +"━┛ ");
	
	
	
	return join(lines);
}

}

