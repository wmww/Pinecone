#include "../../h/utils/stringNumConversion.h"
#include <cstring> // for memcpy

namespace str
{

string charToCppStringLiteralEscaped(unsigned char c)
{
	string out="\\";
	
	for (int i=0; i<3; i++)
	{
		out+=c%8+'0';
		c/=8;
	}
	
	return out;
}

string intToBase62(unsigned int num, int maxDigits)
{
	int i=0;
	string out;
	
	while (num!=0 && (maxDigits<0 || i<maxDigits))
	{
		unsigned int digit=num%62;
		
		if (digit<10)
		{
			out+=(char)(digit+'0');
		}
		else if (digit<10+26)
		{
			out+=(char)(digit-10+'a');
		}
		else if (digit<10+26+26)
		{
			out+=(char)(digit-10-26+'A');
		}
		else
		{
			out+="_"; // this shouldn't happen
		}
		
		num/=62;
		i++;
	}
	
	return out;
}

string ptrToUniqueStr(void* ptrIn, int digits)
{
	unsigned long long data=0;
	memcpy(&data, &ptrIn, sizeof(void*));
	return intToBase62((int)data, digits);
}

}

