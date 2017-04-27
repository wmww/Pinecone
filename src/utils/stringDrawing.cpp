
#include "../../h/utils/stringDrawing.h"
#include "../../h/utils/stringArray.h"

#include "../../h/msclStringFuncs.h"

//#include <math.h>

namespace str
{

// https://en.wikipedia.org/wiki/Box-drawing_character
// │ ─ ╭ ╮ ╯ ╰

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
	
	string bottom;
	string emptyLn;
	
	for (int i=0; i<width; i++)
	{
		bottom+="━";
		emptyLn+=" ";
	}
	
	if (boxName.empty())
	{
		lines.insert(lines.begin(), " ┃ "+bottom+ " ┃ ");
		lines.insert(lines.begin(), " ┏━"+emptyLn+"━┓ ");
	}
	else
	{
		string underline;
		for (int i=0; i<nameWidth; i++)
		{
			underline+="─";
		}
		
		lines.insert(lines.begin(), " ┃"+pad(underline,	width+2, ALIGNMENT_CENTER, " ", "╰─", "─╯")+"┃ ");
		lines.insert(lines.begin(), " ┏"+pad(boxName,	width+2, ALIGNMENT_CENTER, "━", "┥ ", " ┝")+"┓ ");
		lines.insert(lines.begin(), "  "+pad(underline,	width+2, ALIGNMENT_CENTER, " ", "╭─", "─╮")+"  ");
	}
	
	
	lines.push_back(" ┃ "+emptyLn+" ┃ ");
	lines.push_back(" ┗━"+bottom +"━┛ ");
	
	
	
	return join(lines);
}

/*
string makeTreeSection(const string& root, vector<string>& leaves)
{
	return "makeTreeSection not yet implemented";
}
*/

void lightlyBoxStringArray(vector<string>& data)
{
	int width=data.empty()?0:getWidth(data[0]);
	
	for (int i=0; i<(int)data.size(); i++)
	{
		data[i]="│"+data[i]+"│";
	}
	
	string hLine;
	
	for (int i=0; i<width; i++)
	{
		hLine+="─";
	}
	
	data.push_back("╰"+hLine+"╯");
	data.insert(data.begin(), "╭"+hLine+"╮");
}

string getLightlyBoxedString(const string& in)
{
	vector<string> ary;
	splitByLine(ary, in);
	lightlyBoxStringArray(ary);
	return join(ary);
}

string makeRootUpBinaryTree(const string& root, const string& leftLeaf, const string& rightLeaf)
{
	vector<string> leftAry;
	splitByLine(leftAry, leftLeaf);
	
	vector<string> rightAry;
	splitByLine(rightAry, rightLeaf);
	
	/*
	if (rightAry.empty() && !leftAry.empty())
		rightAry.push_back(padString("", getMaxWidth(leftAry)));
	
	if (leftAry.empty() && !rightAry.empty())
		leftAry.push_back(padString("", getMaxWidth(rightAry)));
	*/
	
	if (leftAry.empty())
		leftAry.push_back("");
	
	if (rightAry.empty())
		rightAry.push_back("");
	
	while (leftAry.size()<rightAry.size())
		leftAry.push_back("");
	
	while (rightAry.size()<leftAry.size())
		rightAry.push_back("");
	
	vector<string> rootAry;
	splitByLine(rootAry, root);
	padWidths(rootAry);
	//lightlyBoxStringArray(rootAry);
	int rootWidth=getMaxWidth(rootAry);
	
	padWidths(leftAry, std::max(getMaxWidth(leftAry), rootWidth), ALIGNMENT_CENTER);
	padWidths(rightAry, std::max(getMaxWidth(rightAry), rootWidth), ALIGNMENT_CENTER);
	
	int size=std::max(
			rootWidth,
			(int)(leftAry.empty()?0:leftAry[0].size())
			+
			(int)(rightAry.empty()?0:rightAry[0].size())
		);
	
	padWidths(rootAry, size, ALIGNMENT_CENTER);
	
	for (int i=0; i<(int)leftAry.size(); i++)
	{
		rootAry.push_back(leftAry[i]+rightAry[i]);
	}
	
	return join(rootAry);
}

}

