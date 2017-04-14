#include "../../h/utils/stringArray.h"

namespace str
{

int getMaxWidth(vector<string>& in)
{
	int out=0;
	
	for (int i=0; i<(int)in.size(); i++)
	{
		int w=getWidth(in[i]);
		
		if (w>out)
			out=w;
	}
	
	return out;
}

void padWidths(vector<string>& out, int size, StringPadAlignment alignment, string pad, string leftCap, string rightCap)
{
	if (size<0)
	{
		size=getMaxWidth(out);
	}
	
	for (int i=0; i<(int)out.size(); i++)
	{
		out[i]=str::pad(out[i], size, alignment, pad, leftCap, rightCap);
	}
}

string join(vector<string>& in, string joiner, bool addAtEnd)
{
	string out;
	
	for (int i=0; i<(int)in.size(); i++)
	{
		out+=in[i];
		
		if (addAtEnd || i!=(int)in.size()-1)
			out+=joiner;
	}
	
	return out;
}


}


