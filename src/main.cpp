#include "../h/msclStringFuncs.h"
#include "../h/PineconeParser.h"

#include <iostream>

using std::cout;
using std::endl;

int main(int argc, char ** argv)
{
	cout << "Pinecone transpiler running..." << endl;
	
	string inputFilename;
	
	if (argc>1)
		inputFilename=string(argv[1]);
	else
		inputFilename="../pinecone_source/pinecone.pncn";
	
	string widapLangSource=loadEntireFile(inputFilename, true);
	
	cout << "file contents:" << endl;
	
	cout << putStringInBox(widapLangSource, inputFilename) << endl;
	
	PineconeParser transpiler;
	
	string cppSource=transpiler.resolveProgram(widapLangSource, inputFilename, true);
	
	string outputFilename;
	
	if (argc>2)
		outputFilename=string(argv[2]);
	else
		outputFilename="output.cpp";
	
	cout << "output file:" << endl;
	
	cout << putStringInBox(cppSource, outputFilename) << endl;
	
	cout << "program done" << endl;
	
    return 0;
}


