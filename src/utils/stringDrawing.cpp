
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
	
	data.push_back("╰"+pad("", width, ALIGNMENT_LEFT, "─")+"╯");
	data.insert(data.begin(), "╭"+pad("┴", width, ALIGNMENT_CENTER, "─")+"╮");
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
		
	if (leftAry.empty())
		leftAry.push_back("");
	*/
	
	// make sure left and right arrays are the same size
	while (leftAry.size()<rightAry.size())
		leftAry.push_back("");
	while (rightAry.size()<leftAry.size())
		rightAry.push_back("");
	
	// set up the root array
	vector<string> rootAry;
	splitByLine(rootAry, root);
	if (rootAry.empty())
		rootAry.push_back("");
	padWidths(rootAry);
	
	int rootWidth=getMaxWidth(rootAry);
	//int leafWidth=std::max(std::max(getMaxWidth(leftAry), getMaxWidth(rightAry)), rootWidth)+1;
	
	//padWidths(leftAry, leafWidth, ALIGNMENT_LEFT);
	//padWidths(rightAry, leafWidth, ALIGNMENT_RIGHT);
	
	// make sure everything is padded
	padWidths(leftAry);
	padWidths(rightAry);
	
	//int size=std::max(rootWidth, getMaxWidth(leftAry)+getMaxWidth(rightAry));
	
	// left connection line
	if (!leftAry.empty())
	{
		int endXPos=getGlyphPosOf(leftAry[0], "┴");
		if (endXPos>0)
		{
			int middleYPos=rootAry.size()/2;
			rootAry[middleYPos]="┤"+sub(rootAry[middleYPos], 1, -1);
			int leftPadWidth=getMaxWidth(leftAry)-rootWidth/2;
			if (endXPos+1>leftPadWidth)
				leftPadWidth=endXPos+1;
			for (int i=0; i<(int)rootAry.size(); i++)
			{
				if (i<middleYPos)
					rootAry[i]=padString("", leftPadWidth)+rootAry[i];
				else if (i==middleYPos)
					rootAry[i]=padString("", endXPos)+"╭"+padString("", leftPadWidth-endXPos-1, ALIGNMENT_LEFT, "─")+rootAry[i];
				else
					rootAry[i]=padString("", endXPos)+"│"+padString("", leftPadWidth-endXPos-1)+rootAry[i];
			}
		}
	}
	
	for (int i=0; i<(int)leftAry.size(); i++)
	{
		rootAry.push_back(leftAry[i]+rightAry[i]);
	}
	
	padWidths(rootAry);
	
	return join(rootAry);
}

}

