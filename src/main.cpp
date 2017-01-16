#include "../h/msclStringFuncs.h"
#include "../h/PineconeProgram.h"
#include "../h/ErrorHandler.h"

#include <iostream>

using std::cout;
using std::endl;

int main(int argc, char ** argv)
{
	cout << "Pinecone v" << VERSION_X << "." << VERSION_Y << "." << VERSION_Z << " running..." << endl;
	
	string inputFilename;
	
	if (argc>1)
		inputFilename=string(argv[1]);
	else
		inputFilename="other/pinecone.pncn";
	
	PineconeProgram parser;
	
	parser.resolveProgram(inputFilename, true);
	
	if (error.getIfErrorLogged())
	{
		cout << endl << ">>>>>>    execution abouted due to previous error    <<<<<<" << endl;
	}
	else
	{
		cout << endl << "executing program..." << endl << endl;
		
		parser.execute();
	}
	
	cout << endl << "all done" << endl;
	
	string outputFilename;
	
	/*if (argc>2)
		outputFilename=string(argv[2]);
	else
		outputFilename="output.cpp";
	
	cout << "output file:" << endl;
	
	cout << putStringInBox(cppSource, outputFilename) << endl;
	
	cout << "program done" << endl;*/
	
	return 0;
}


