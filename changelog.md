# Pinecone Changelog
This file documents all major changes to the Pinecone langauge.
Every new version gets a number of minor bug fixes, so they are not included

### v0.5.1 (April 20, 2017)

* Fixed silent error when file fails to load
* Fixed known bug with functions that take one named argument
* Fixed known bug that prevented `?` being useed in a ternary expression
* Improved output of testing script
* Internal codebase cleanup

## v0.5.0 (April 14, 2017)

* Made '&&' and '||' use short circuiting
* Improved testing system
* Added support for Windows
* __Added Whatev type__ for compile time dynamic typing and implicit function return type

## v0.4.0 (Mar 26, 2017)

* Added command line options for transpileing
* __Fully implemented transpiler to C++__
* Disabled implicit conversion between two structs (if neither is an anonymous tuple)
* Added `!=` operator
* Wrote more example programs including a brainfuck interpreter
* Fixed bug that prevented accessing multiple layers of structures with dot ('struct.data.func: arg' now works)
* Added a wide range of unit tests and integration tests
* Added escape character support to Strings
* Added support for running shell commands
* Added explicit conversion of primitive types to Strings
* Improved tutorial navigation

## v0.3.0 (Jan 18, 2017)

* __Added string literal support__
* Added various functions and operators for strings
* Fixed globals

### v0.2.1 (Jan 16, 2017)

* Fixed bug introduced in v0.2.0 with functions
* Fixed bug with line numbers in error messages

## v0.2.0 (Jan 16, 2017)

* Added file import feature
* Flipped open and close block comments
* Removed debugging info by default
* Added command line args

## v0.1.0 (Jan 15, 2017)

* __Created Pinecone__

## v0.0.0

* Pinecone did not exist
