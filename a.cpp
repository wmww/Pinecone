// this C++ code is transpiled from Pinecone
// Pinecone v0.3.0 was used

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

struct String
{
	int _size;
	unsigned char * _data;

	String() {}

	String(int _size_in, unsigned char * _data_in)
	{
		_size = _size_in;
		_data = _data_in;
	}
};

char * cStr(String in);
String concatStr(String a, String b);
bool eqStr(String a, String b);
String getInputLine(String prompt);
String pnStr(const char * in);
String subStr(String in, int a, int b);
String letterToMorse(int _size, unsigned char * _data);
String stringToMorse(int _size, unsigned char * _data);

bool quit;
String inStr;

char * cStr(String in)
{
	char * out = (char *)malloc(in._size+1);
	memcpy(out, in._data, in._size);
	out[in._size] = 0;
	return out;
}

String concatStr(String a, String b)
{
	int newSize = a._size + b._size;
	unsigned char * newData = (unsigned char *)malloc(newSize);
	memcpy(newData, a._data, a._size);
	memcpy(newData + a._size, b._data, b._size);
	return String(newSize, newData);
}

bool eqStr(String a, String b)
{
	if (a._size != b._size)
		return false;
	int size = a._size;
	for (int i = 0; i < size; i++)
	{
		if (a._data[i] != b._data[i])
			return false;
	}
	return true;
}

String getInputLine(String prompt)
{
	printf(cStr(prompt));
	fflush(stdout);
	size_t bufferSize = 4096;
	char buffer[bufferSize];
	if (fgets(buffer, bufferSize, stdin))
	{
		int size = strlen(buffer) - 1; // remove null
		unsigned char * data = (unsigned char *)malloc(size);
		memcpy(data, buffer, size);
		return String(size, data);
	}
	else
	{
		return String(0, NULL);
	};
}

String pnStr(const char * in)
{
	int size = 0;
	while (in[size]) size++;
	unsigned char * data = (unsigned char *)malloc(size);
	memcpy(data, in, size);
	return String(size, data);
}

String subStr(String in, int a, int b)
{
	int size = b - a;
	unsigned char * data = (unsigned char *)malloc(size);
	memcpy(data, in._data + a, size);
	return String(size, data);
}

String letterToMorse(int _size, unsigned char * _data)
{
	String out;
	int i;
	int j;
	bool found;
	
	{
		out = (pnStr(""));
		{
			i = (0);
			while ((i) < (pnStr("a.- b-... c-.-. d-.. e. f..-. g--. h.... i.. j.--- k-.- l.-.. m-- n-. o--- p.--. q--.- r.-. s... t- u..- v...- w.-- x-..- y-.-- z--.. 1.---- 2..--- 3...-- 4....- 5..... 6-.... 7--... 8---.. 9----. 0----- ..-.-.- ?..--.. ")._size))
			{
				if (eqStr(subStr(pnStr("a.- b-... c-.-. d-.. e. f..-. g--. h.... i.. j.--- k-.- l.-.. m-- n-. o--- p.--. q--.- r.-. s... t- u..- v...- w.-- x-..- y-.-- z--.. 1.---- 2..--- 3...-- 4....- 5..... 6-.... 7--... 8---.. 9----. 0----- ..-.-.- ?..--.. "), i, (i) + (1)), String(_size, _data)))
				{
					{
						j = ((i) + (1));
						while (!(eqStr(subStr(pnStr("a.- b-... c-.-. d-.. e. f..-. g--. h.... i.. j.--- k-.- l.-.. m-- n-. o--- p.--. q--.- r.-. s... t- u..- v...- w.-- x-..- y-.-- z--.. 1.---- 2..--- 3...-- 4....- 5..... 6-.... 7--... 8---.. 9----. 0----- ..-.-.- ?..--.. "), j, (j) + (1)), pnStr(" "))))
						{
							j = ((j) + (1));
						}
						out = (subStr(pnStr("a.- b-... c-.-. d-.. e. f..-. g--. h.... i.. j.--- k-.- l.-.. m-- n-. o--- p.--. q--.- r.-. s... t- u..- v...- w.-- x-..- y-.-- z--.. 1.---- 2..--- 3...-- 4....- 5..... 6-.... 7--... 8---.. 9----. 0----- ..-.-.- ?..--.. "), (i) + (1), j));
						found = (true);
					}
				}
				i = ((i) + (1));
			}
		}
		return out;
	}
}

String stringToMorse(int _size, unsigned char * _data)
{
	String out;
	int i;
	String letter;
	
	{
		out = (pnStr(""));
		{
			i = (0);
			while ((i) < (_size))
			{
				{
					letter = (subStr(String(_size, _data), i, (i) + (1)));
					if (eqStr(letter, pnStr(" ")))
					{
						out = (concatStr(out, pnStr("    ")));
					}
					else
					{
						out = (concatStr(concatStr(out, letterToMorse(letter._size, letter._data)), pnStr(" ")));
					}
				}
				i = ((i) + (1));
			}
		}
		return out;
	}
}

int main()
{
	{
		quit = (false);
		while (!(quit))
		{
			{
				inStr = (getInputLine(pnStr("enter some text or type quit: ")));
				if (eqStr(inStr, pnStr("quit")))
				{
					quit = (true);
				}
				else
				{
					printf("%s\n", cStr(stringToMorse(inStr._size, inStr._data)));
				}
			}
		}
		printf("\n");
	}
	return 0;
}

