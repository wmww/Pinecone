#include "../h/msclStringFuncs.h"
#include "../h/PineconeProgram.h"
#include "../h/ErrorHandler.h"

#include <iostream>

using std::cout;
using std::endl;

struct Flags
{
	vector<string> inFiles;
	bool debug=false;
	bool showCpp=false;				// if to show transpiled C++ code
	bool help=false;
	bool version=false;
	bool flagError=false;			// if to quit immediately, this is set if there is an unrecognised flag
};

Flags getFlags(int argc, char ** argv);

int main(int argc, char ** argv)
{
	Flags flags=getFlags(argc, argv);
	
	if (flags.flagError)
	{
		cout << "try 'pinecone -h' for help" << endl;
		return 0;
	}
	
	if (flags.help)
	{
		cout << "Pinecone v" << VERSION_X << "." << VERSION_Y << "." << VERSION_Z << endl;
		cout << "usage: pinecone [options] [source file]" << endl;
		cout << "options: " << endl;
		cout << "-v, -version     display the version of Pinecone" << endl;
		cout << "-debug           display debugging info and then run the program" << endl;
		cout << "-cpp             instead of running the program, show transpiled C++ code" <<
				"                 not yet fully implemented" << endl;
		cout << "-h, -help        display this help and quit" << endl;
		return 0;
	}
	
	if (flags.version)
	{
		cout << "Pinecone version " << VERSION_X << "." << VERSION_Y << "." << VERSION_Z << endl;
		return 0;
	}
	
	PineconeProgram program;
	
	if (flags.inFiles.empty())
	{
		cout << "no source file specified" << endl;
		cout << "try 'pinecone -h' for help" << endl;
		return 0;
	}
	else if (flags.inFiles.size()>1)
	{
		cout << "multiple source files specified, Pinecone does not currently support this" << endl;
		cout << "try 'pinecone -h' for help" << endl;
		return 0;
	}
	
	program.resolveProgram(flags.inFiles[0], flags.debug);
	
	if (error.getIfErrorLogged())
	{
		if (flags.debug)
			cout << endl << ">>>>>>    execution abouted due to previous error    <<<<<<" << endl;
		else
			cout << "program not executed due to errors" << endl;
	}
	else if (flags.showCpp)
	{
		cout << putStringInBox(program.getCpp(), "C++ code", true) << endl;
	}
	else
	{
		if (flags.debug)
			cout << endl << "executing program..." << endl << endl;
		
		program.execute();
	}
	
	if (flags.debug)
		cout << endl << "all done" << endl;
	
	return 0;
}

Flags getFlags(int argc, char ** argv)
{
	Flags flags;
	
	for (int i=1; i<argc; i++)
	{
		string arg(argv[i]);
		
		if (arg.size()>1 && arg[0]=='-')
		{
			string flag=arg.substr(1, string::npos);
			
			if (flag=="debug")
			{
				flags.debug=true;
			}
			else if (flag=="cpp")
			{
				flags.showCpp=true;
			}
			else if (flag=="v" || flag=="version")
			{
				flags.version=true;
			}
			else if (flag=="h" || flag=="help")
			{
				flags.help=true;
			}
			else
			{
				cout << "unknown flag '"+flag+"'" << endl;
				flags.flagError=true;
			}
		}
		else
		{
			flags.inFiles.push_back(arg);
		}
	}
	
	return flags;
}


