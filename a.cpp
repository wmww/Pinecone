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

struct nm_glc
{
	int x;
	int y;
	int z;

	nm_glc() {}

	nm_glc(int x_in, int y_in, int z_in)
	{
		x = x_in;
		y = y_in;
		z = z_in;
	}
};

String asciiToStr(int in);
char * cStr(String in);
String concatStr(String a, String b);
bool eqStr(String a, String b);
String intToStr(int in);
String pnStr(const char * in);
String runCmd(String cmd);
String subStr(String in, int a, int b);
void runTest(String filepath, String name, bool printDetails);
void runTestFolder(String folder, bool printDetails);

String options;

String asciiToStr(int in)
{
	unsigned char * data = (unsigned char *)malloc(1);
	*data = in;
	return String(1, data);
}

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

String intToStr(int in)
{
	bool neg = in < 0;
	if (neg) in *= -1;
	int val = in;
	int size = 0;
	while (val)
	{
		size++;
		val /= 10;
	}
	if (size == 0)
		size = 1;
	if (neg)
		size++;
	unsigned char * data = (unsigned char *)malloc(size);
	val = in;
	for (int i = 0; i<(neg ? size-1 : size); i++)
	{
		data[size-i-1] = (val % 10) + '0';
		val /= 10;
	}
	if (neg)
		data[0] = '-';
	return String(size, data);
}

String pnStr(const char * in)
{
	int size = 0;
	while (in[size]) size++;
	unsigned char * data = (unsigned char *)malloc(size);
	memcpy(data, in, size);
	return String(size, data);
}

String runCmd(String cmd)
{
	String result = pnStr("");
	FILE* pipe = popen(cStr(cmd), "r");
	if (!pipe)
		return result;
	while (!feof(pipe)) {
		char c;
		if ((c=getc(pipe)) != EOF)
		{
			result=concatStr(result, asciiToStr(c));
		}
	}
	pclose(pipe);
	return result;
}

String subStr(String in, int a, int b)
{
	int size = b - a;
	unsigned char * data = (unsigned char *)malloc(size);
	memcpy(data, in._data + a, size);
	return String(size, data);
}

void runTest(String filepath, String name, bool printDetails)
{
	{
		String cmd;
		cmd = (concatStr(concatStr(pnStr("./pinecone "), filepath), options));
		String out;
		out = (runCmd(cmd));
		printf("%s\n", cStr(concatStr(pnStr("\x09\x09command: "), cmd)));
		bool quit;
		quit = (false);
		int i;
		i = (0);
		String target;
		target = (pnStr(""));
		String actual;
		actual = (pnStr(""));
		while (!(quit))
		{
			if (((i) + (pnStr("_____\n")._size)) >= (out._size))
			{
				{
					target = (pnStr("no target"));
					if ((out._size) == (0))
					{
						actual = (concatStr(concatStr(pnStr("no output for '"), filepath), pnStr("'")));
					}
					else
					{
						actual = (out);
					}
					quit = (true);
				}
			}
			else
			{
				if (eqStr(subStr(out, i, (i) + (pnStr("_____\n")._size)), pnStr("_____\n")))
				{
					{
						target = (subStr(out, 0, i));
						actual = (subStr(out, (i) + (pnStr("_____\n")._size), out._size));
						quit = (true);
					}
				}
				else
				{
					i = ((i) + (1));
				}
			}
		}
		if (eqStr(target, actual))
		{
			printf("%s\n", cStr(concatStr(concatStr(pnStr(".\x09"), name), pnStr(" passed"))));
		}
		else
		{
			{
				printf("%s\n", cStr(concatStr(concatStr(pnStr("X\x09"), name), pnStr(" failed"))));
				if (printDetails)
				{
					{
						printf("%s\n", cStr(pnStr("_____________________________________________\n")));
						printf("%s\n", cStr(pnStr("output:")));
						printf("%s\n", cStr(actual));
						printf("%s\n", cStr(pnStr("instead of:")));
						printf("%s\n", cStr(target));
						printf("%s\n", cStr(pnStr("_____________________________________________\n")));
					}
				}
			}
		}
	}
}

void runTestFolder(String folder, bool printDetails)
{
	{
		printf("%s\n", cStr(concatStr(folder, pnStr(":"))));
		String filepath;
		filepath = (concatStr(pnStr("examples/tests/"), folder));
		String cmd;
		cmd = (concatStr(concatStr(pnStr("ls "), filepath), pnStr("/ | grep .pn")));
		String allFiles;
		allFiles = (runCmd(cmd));
		int i;
		i = (0);
		{
			int j;
			j = (0);
			while ((j) < (allFiles._size))
			{
				if (((j) == ((allFiles._size) - (1))) || (eqStr(subStr(allFiles, j, (j) + (1)), pnStr("\n"))))
				{
					{
						String name;
						name = (subStr(allFiles, i, j));
						runTest(concatStr(concatStr(filepath, pnStr("/")), name), subStr(name, 0, (name._size) - (3)), printDetails);
						i = ((j) + (1));
					}
				}
				j = ((j) + (1));
			}
		}
		printf("\n");
	}
}

int main()
{
	{
		options = (pnStr(""));
		if (true)
		{
			options = (concatStr(options, pnStr(" -e")));
		}
		printf("%s\n", cStr(concatStr(concatStr(concatStr(concatStr(concatStr(pnStr("pinecone v"), intToStr(nm_glc(0, 3, 0).x)), pnStr(".")), intToStr(nm_glc(0, 3, 0).y)), pnStr(".")), intToStr(nm_glc(0, 3, 0).z))));
		printf("\n");
		printf("%s\n", cStr(pnStr("unit tests:")));
		printf("\n");
		runTestFolder(pnStr("unit"), true);
		runTestFolder(pnStr("regression"), true);
		runTestFolder(pnStr("integration"), true);
		runTestFolder(pnStr("unfixed"), false);
		printf("%s\n", cStr(pnStr("tests done")));
	}
	return 0;
}

