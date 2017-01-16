# The Pinecone Programming Language
**_Built from the ground up to be fast, concise and intuitive._**


## About
Pinecone is a programming language currently under development by William W Wold. Work on the language began on October 4th, 2016. Right now effort is focused on writing an interpreter in C++, although Pinecone is designed to eventually be compiled or transpiled to C++. The language is written from scratch (it includes an integrated lexer, parser and interpreter).

## Why?
This is probably the most common reaction to hearing about a new language. I'm not guaranteeing I can justify Pinecone's existence to you, but here is my best attempt.

Pinecone will be similar in capabilities to modern object oriented compiled languages such as C++, Swift and Rust. It's primary attraction is the simplicity and consistency of it's syntax. There are as few as possible symbols and keywords, and they are flexible enough to work in almost any context. Here are some examples of how Pinecone is different from other popular languages:

* Calling a function that takes no arguments is the same syntax as accessing a variable (just writing it's name).
* Calling a function that takes one argument is the same syntax as setting a variable (`funcOrVar: input`).
* Calling a function that takes multiple arguments is the same syntax as setting a tuple (`funcOrTuple: (input1, input2)`).
* Variable creation is implicit, just set a variable and it is created.
* Variables are statically typed, but type deduction is automatic.
* White space is ignored _and_ semicolons are not necessary
* `:` is used for assignment, which leaves `=` available for comparison (rather then `==`).
* Tuples, structs and classes are all basically the same thing
* Functions can be sent arguments from the left side, right side or both (`inputLeft.function: inputRight`), which is used for methods but can also allow you to define functions for any type (even primitive) that act like methods
* Program control is done with operators instead of keywords (`?` instead of `if`)

## Current State
the features that are currently implemented are as follows:
* Primitive data types `Bool`, `Int` and `Dub`
* All the operators you would expect (`+`, `*`, `%`, `:`, `=`, `>`, `<=`, `&&`, etc.)
* Single and multi line comments
* Flow control (if, if/else, while loop, for loop)
* Lazily evaluated compile time constants
* Tuples and data structures
* Functions that can take left and right inputs, and can return values

the following features are either coming soon, or implemented now but broken:
* Global variables
* Pass-by-reference
* Proper classes (pass-by-reference needed)
* Arrays for any type
* Strings
* Operator overloading
* Transpiling to C++ (currently the language is only interpreted)

__here is an example of a very simple Pinecone program:__
It prints out all the fibonacci numbers that are less then 100 and even
```
a: 1
b: 0

a<100 @
(
	a%2=0 ?
		print: a

	tmp: a
	a: a+b
	b: tmp
)
```

If you want to program in Pinecone now, see the tutorials folder for how to get started.
