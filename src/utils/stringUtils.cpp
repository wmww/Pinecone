#include "../../h/utils/stringUtils.h"
#include <math.h>

namespace str
{

void splitBy(vector<string>& out, const string& in, const string& splitter, bool keepSplitter)
{
	int i=0;
	int start=0;
	
	while (i<=(int)in.size()-(int)splitter.size())
	{
		if (subMatches(in, i, splitter))
		{
			if (keepSplitter)
				nextGlyph(i, in);
			
			out.push_back(in.substr(start, i-start));
			
			if (!keepSplitter)
				nextGlyph(i, in);
			
			start=i;
		}
		else
		{
			nextGlyph(i, in);
		}
	}
	
	if ((int)in.size()!=start)
		out.push_back(in.substr(start, in.size()-start));
}

string pad(const string& in, int size, StringPadAlignment alignment, string pad, string leftCap, string rightCap)
{
	int capWidth=getWidth(leftCap)+getWidth(rightCap);
	int inSize=getWidth(in);
	int padSize=size-(inSize+capWidth);
	
	if (padSize<0) // need to chop
	{
		if (size-capWidth>=1)
		{
			if (alignment==ALIGNMENT_RIGHT)
			{
				return leftCap+"…"+sub(in, inSize-(size-capWidth-1), inSize)+rightCap;
			}
			else
			{
				return leftCap+sub(in, 0, size-capWidth-1)+"…"+rightCap;
			}
		}
		else
		{
			return sub(leftCap+rightCap, 0, size);
		}
	}
	else if (padSize==0) // input size was exactly right
	{
		return leftCap+in+rightCap;
	}
	else // need to pad
	{
		if (alignment==ALIGNMENT_CENTER) // center alignment
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
			
			if (alignment==ALIGNMENT_RIGHT) // right align
				return padStr+leftCap+in+rightCap;
			else // left align
				return leftCap+in+rightCap+padStr;
		}
	}
}

string tabsToSpaces(const string& in, int tabWidth)
{
	int widthSinceNewline=0;
	string out;
	int bytePos=0;
	int start=0;
	
	while (bytePos<(int)in.size())
	{
		if (subMatches(in, bytePos, "\t"))
		{
			out+=in.substr(start, bytePos-start);
			string spacer;
			int spaces=tabWidth-(widthSinceNewline%tabWidth);
			for (int i=0; i<spaces; i++)
				spacer+=" ";
			out+=spacer;
			widthSinceNewline+=spaces;
			nextGlyph(bytePos, in);
			start=bytePos;
		}
		else
		{
			widthSinceNewline++;
			if (subMatches(in, bytePos, "\n"))
			{
				widthSinceNewline=0;
			}
			nextGlyph(bytePos, in);
		}
	}
	
	out+=in.substr(start, bytePos-start);
	
	return out;
}

int getGlyphPosOf(const string& in, string pattern)
{
	int glyph=0;
	int byte=0;
	
	while (byte<(int)in.size())
	{
		if (subMatches(in, byte, pattern))
			return glyph;
		
		nextGlyph(byte, in);
		glyph++;
	}
	
	return -1;
}

}


