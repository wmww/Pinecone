
#pragma once

/// This string util header will slowly replace msclStringFuncs.h

#include <string>
using std::string;

#include <vector>
using std::vector;

namespace str
{

inline void nextGlyph(int& out, const string& in);

inline int getWidth(const string& in);

// returns the byte location of the given glyph
inline int seek(const string& in, int distGlyph, int startPosByte=0);

// currently is a straight equals, but in the future could do things such as evaluate strings with different types of newlines to equal
inline bool subMatches(const string& in, int posBytes, const string& sub);

// if endGlyph is -1 it takes till the end of the string
inline string sub(const string& in, int startGlyph, int endGlyph);

inline bool matches(const string& a, const string& b);

inline bool hasPrefix(const string& in, const string& prefix);

inline bool hasSuffix(const string& in, const string& suffix);

// returns the BYTE location (not glyph location), or -1 if pattern doesn't appear
//int searchFor(const string& in, string pattern, int startByte);

// returns the glyph position of the first occurrence of pattern, or -1 if it doesn't appear
int getGlyphPosOf(const string& in, string pattern);

string tabsToSpaces(const string& in, int tabWidth=4);

enum StringPadAlignment {ALIGNMENT_LEFT=1, ALIGNMENT_CENTER=0, ALIGNMENT_RIGHT=-1};

// alignment: 1 = left, -1 = right, 0 = center
// pad is assumed to be of width 1
string pad(const string& in, int size, StringPadAlignment alignment=ALIGNMENT_LEFT, string pad=" ", string leftCap="", string rightCap="");


/// inline implementations

inline void nextGlyph(int& out, const string& in)
{
	do {
		out++;
	} while (out<(int)in.size() && (in[out] & 0x80) && !(in[out] & 0x40));
}

inline int getWidth(const string& in)
{
	int glyphPos=0;
	int bytePos=0;
	
	while (bytePos<(int)in.size())
	{
		nextGlyph(bytePos, in);
		glyphPos++;
	}
	
	return glyphPos;
}

inline int seek(const string& in, int distGlyph, int startPosByte)
{
	int i=startPosByte;
	
	while (distGlyph>0)
	{
		nextGlyph(i, in);
		
		distGlyph--;
	}
	
	return i;
}

inline bool subMatches(const string& in, int posBytes, const string& sub)
{
	if (posBytes<0  || sub.size()+posBytes>in.size())
		return false;
	
	for (int i=0; i<(int)sub.size(); i++)
	{
		if (in[i+posBytes]!=sub[i])
			return false;
	}
	
	return true;
}

inline string sub(const string& in, int startGlyph, int endGlyph)
{
	int startByte=seek(in, startGlyph);
	int endByte=(endGlyph < 0 ? (int)in.size() : seek(in, endGlyph-startGlyph, startByte));
	return in.substr(startByte, endByte-startByte);
}

inline bool matches(const string& a, const string& b)
{
	if (a.size()!=b.size())
		return false;
	
	return subMatches(a, 0, b);
}

inline bool hasPrefix(const string& in, const string& prefix)
{
	return subMatches(in, 0, prefix);
}

inline bool hasSuffix(const string& in, const string& suffix)
{
	return subMatches(in, in.size()-suffix.size(), suffix);
}

}

#include "stringArray.h"
#include "stringDrawing.h"
