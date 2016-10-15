# The Pinecone Programming Language
**_Built from the ground up to be fast, concise and intuitive._**


## About
Pinecone is a programming language currently under development by William W Wold. Work on the language began on October 4th, 2016. Right now effort is focused on writing an interpreter in C++, although Pinecone is designed to eventually be compiled or transpiled to C. The language is currently written from scratch (it includes an intagrated lexer, parser and interpreter) although I am considering switching to Bison for parsing.

## Why?
This is probably the most common reaction to hearing about a new language. I'm not guaranteeing I can justify Pinecone's existence to you, but here is my best attempt.

Pinecone will be similar to modern object oriented compiled languages such as C++, Swift and Rust. It's primary attraction will be the simplicity and consistency of it's syntax. There will be as few as possible different things a programmer can do, and those things will be flexible enough to work in almost any context. Here are some examples of how Pinecone is different from other popular languages:

* Calling a function that takes no arguments is the same syntax as accessing a variable (just writing it's name).
* Calling a function that takes one argument is the. same syntax as setting a variable (identifier: input).
* Calling a function that takes multiple arguments is the same syntax as setting a tuple (identifier: (input1, input2)).
* Variable creation is implicit, just set a variable and it is created.
* Variables are typed, but type deduction is automatic.
* Whitespace is ignored AND semicolons are not necessary (even when several expressions are on the same line)
* " : " is used for assignment which leaves " = " available for comparison (rather then " == ").
* Tuples, structs and classes are all the same thing
* Functions can be sent arguments from the left side, right side or both (inputLeft.function: inputRight), which is used for class functions but can also allow you to define functions for any type that act like class functions
* Program control is done with operators instead of keywords (? instead of if)

## Current State
Right now Pinecone isn't good for a whole lot, but that shouldn't be surprising considering the first line of code for it was written less then a month ago. What I have working so far is a few primitive data types, a few operators and a print function. The following code will currently run correctly:

	a: 7.5          # init a as a Dub
	b: 4            # init b as an Int
	var: a+b        # init var as a Dub to value 11.5
	var: var-(b-2)  # set var to 9.5
	print: var      # print 9.5 to stdout

And that is just about a complete demonstration of currently implemented features.

## What's next
Right I'm working primarily on structuring the parser and the interpreter. Functions are also a big feature that I'd like to get out soon, as well as basic flow control (if/then, loops, etc.). Strings will likely come slightly later, but they are also very important. When I have all these things, the language will actually be usable. Then it will just be a matter of making it better.
