#include "../h/msclStringFuncs.h"
#include "../h/PineconeProgram.h"
#include "../h/ErrorHandler.h"

#include <iostream>

using std::cout;
using std::endl;
using std::vector;
using std::string;

vector <string> cmdLineArgs;

struct Flags
{
	string myPath;					// path to the pinecone exeutable that is now running
	vector<string> inFiles;			// all the input files
	bool debug=false;				// if to show debugging info
	bool help=false;				// if to show help message
	bool version=false;				// if to show version message
	bool runInterpreted=true;		// if to run the program in the interpreter
	string cppOutFile="";			// output file for transpiled C++ code, empty if flag not set
	string binOutFile="";			// binary executable output file, empty if flag not set
	bool runCompiled=false;			// if to run the program compiled
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
		cout << "usage: pinecone [options] [source file] [options]" << endl;
		cout << "options: " << endl;
		cout << "-v, -version      display the version of Pinecone" << endl;
		cout << "-d, -debug        display debugging info before running the program" << endl;
		cout << "-r, -run          run the program with the interpreter" << endl;
		cout << "                    active by default if no transpiling commands are present" << endl;
		cout << "                    currently, anything after -r is ignored" << endl;
		cout << "-cpp [file]       transpile to C++ and save the output in the given file" << endl;
		cout << "-bin [file]       transpile, compile with GCC and save the binary" << endl;
		cout << "-e, -execute      transpile, compile and execute the binary" << endl;
		cout << "                    any combination of -cpp, -bin and -e can be used" << endl;
		cout << "                    like -r, anything after -e is ignored" << endl;
		cout << "-h, -help         display this help and quit" << endl;
		cout << endl;
		cout << endl;
		
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
	
	if (flags.runInterpreted)
	{
		if (error.getIfErrorLogged())
		{
			if (flags.debug)
				cout << endl << ">>>>>>    execution aborted due to previous error    <<<<<<" << endl;
			else
				cout << "program not executed due to errors" << endl;
		}
		else
		{
			if (flags.debug)
				cout << endl << "running program..." << endl << endl;
			
			program.execute();
		}
	}
	
	if (!flags.cppOutFile.empty() || !flags.binOutFile.empty() || flags.runCompiled)
	{
		string cppCode=program.getCpp();
		
		if (error.getIfErrorLogged())
		{
			if (flags.debug)
				cout << endl << ">>>>>>    transpiling failed    <<<<<<" << endl;
			else
				cout << "transpiling failed" << endl;
		}
		else
		{
			string cppFileName=flags.cppOutFile;
			
			if (cppFileName.empty())
				cppFileName="tmp_pn_transpiled.cpp";
			
			if (flags.debug)
				cout << endl << putStringInBox(cppCode, "C++ code", true, false, -1) << endl;
			
			writeFile(cppFileName, cppCode, flags.debug);
			
			if (!flags.binOutFile.empty() || flags.runCompiled)
			{
				string binFileName=flags.binOutFile;
				
				if (binFileName.empty())
					binFileName="tmp_pn_compiled";
				
				string cmd;
				cmd="g++ -std=c++11 '"+cppFileName+"' -o '"+binFileName+"'";
				
				if (flags.debug)
					cout << "running '"+cmd+"'" << endl;
				
				runCmd(cmd, true);
				
				if (flags.runCompiled)
				{
					if (flags.debug)
						cout << endl;
					
					cmd = "./"+binFileName + " --running-from-pinecone " + str::join(cmdLineArgs, " ", false);
					
					if (flags.debug)
						cout << "running '"+cmd+"'" << endl << endl;
					
					runCmd(cmd, true);
				}
				
				if (flags.binOutFile.empty())
					remove(binFileName.c_str());
			}
			
			if (flags.cppOutFile.empty())
				remove(cppFileName.c_str());
		}
	}
	
	if (flags.debug)
		cout << endl << "all done" << endl;
	
	return 0;
}

Flags getFlags(int argc, char ** argv)
{
	bool after = false;
	Flags flags;
	
	for (int i=1; i<argc; i++)
	{
		string arg(argv[i]);
		if (!after)
		{
			if (arg.size()>1 && arg[0]=='-')
			{
				string flag=arg.substr(1, string::npos);

				if (flag=="d" || flag=="debug")
				{
					flags.debug=true;
				}
				else if (flag=="v" || flag=="version")
				{
					flags.version=true;
				}
				else if (flag=="h" || flag=="help")
				{
					flags.help=true;
				}
				else if (flag=="r" || flag=="run")
				{
					flags.runCompiled=false;
					flags.runInterpreted=true;
					after = true;
				}
				else if (flag=="cpp")
				{
					if (i+1>=argc)
					{
						cout << "output file must follow '-cpp' flag";
						flags.flagError=true;
					}

					flags.runInterpreted=false;

					flags.cppOutFile=string(argv[i+1]);

					i++;
				}
				else if (flag=="bin")
				{
					if (i+1>=argc)
					{
						cout << "output file must follow '-bin' flag";
						flags.flagError=true;
					}

					flags.runInterpreted=false;

					flags.binOutFile=string(argv[i+1]);

					i++;
				}
				else if (flag=="e" || flag=="execute")
				{
					flags.runCompiled=true;
					flags.runInterpreted=false;
					after = true;
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
				cmdLineArgs.push_back(arg);
			}
		}
		else
		{
			cmdLineArgs.push_back(arg);
		}
	}
	
	return flags;
}


