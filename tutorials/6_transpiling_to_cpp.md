# Transpiling to C++

Up until now you have been running your Pinecone code the default way, which is currently with the interpreter. Pinecone can also transpile your code to C++. Here I will be explaining how to do that and what the pros and cons of transpiling are.

## What is Interpreting, Compiling and Transpiling?
To compile is to turn one language into another. Generally this means turn a mid level human readable language (C, C++, Rust, etc.) into complex-for-us-but-easy-for-computers machene code that can be executed. When a language (such as Python, Ruby, JavaScript, etc.) is interpreted, the interpreter walks through the source code line by line and runs it as it goes. There are pros and cons of both compiling and interpreting. Generally, interpreting gives more flexibility while compiling gives more performence. Finally, transpiling is technically a subset of compileing. It is converting between two languages of roughly equal complexity. For example, you may want to transpile from a newer version of JS to an older one for compatibility reasons.

## How This Relates to Pinecone
Pinecone has been designed from the begining with the restraints of a compiled langauge in mind. In the begining I implemented the language as interpreted, and the interpreter is still the default way to run Pinecone, but now you can also transpile to C++. This gives your peograms a huge speed boost. The interpreter is default because it doesn't need to write files to disk, it doesn't need an external compiler and there are some known rare cases where transpileing doesn't work.

## Why Transpile to C++ Instead of Compiling Directly
I tried to write a compiler with LLVM, but it was hard so I gave up.

## Using the Transpiler
There are three command line arguments relevent transpiling `-cpp`, `-bin` and `-e`. These arguments can be used any combonation and if any are preasent, transpiling will be used instead of interpreting. `-cpp` and `-bin` should each be followed by a filepath which the transpiled source code and compiled binary executable will be saved to repectivly. `-e` should be used last, as subsequent arguments will be ignored. It means to actually execute the program, rather then just saving the output files. If cpp or bin file is not specified, they will be saved to a temporary file in your current directory which should be deleted automatically when the program is done. For compiling to a binary executable, and thus executing, the GCC compiler must be installed on your system.

[index](index.md) | [next: Temporary Hacks ->](7_temporary_hacks.md)
