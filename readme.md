# The Pinecone Programming Language
**_Built from the ground up to be fast, concise and intuitive._**


## About
Pinecone is a programming language currently under development by William W Wold. Work on the language began on October 4th, 2016. Right now effort is focused on writing an interpreter in C++, although Pinecone is designed to eventually be compiled or transpiled to C. The language is written from scratch (it includes an integrated lexer, parser and interpreter).

## Why?
This is probably the most common reaction to hearing about a new language. I'm not guaranteeing I can justify Pinecone's existence to you, but here is my best attempt.

Pinecone will be similar in capabilities to modern object oriented compiled languages such as C++, Swift and Rust. It's primary attraction will be the simplicity and consistency of it's syntax. There will be as few as possible different things a programmer can do, and those things will be flexible enough to work in almost any context. Here are some examples of how Pinecone is different from other popular languages:

* Calling a function that takes no arguments is the same syntax as accessing a variable (just writing it's name).
* Calling a function that takes one argument is the same syntax as setting a variable (funcOrVar: input).
* Calling a function that takes multiple arguments is the same syntax as setting a tuple (funcOrTuple: (input1, input2)).
* Variable creation is implicit, just set a variable and it is created.
* Variables are typed, but type deduction is automatic.
* Whitespace is ignored _and_ semicolons are not necessary (even when several expressions are on the same line)
* " : " is used for assignment which leaves " = " available for comparison (rather then " == ").
* Tuples, structs and classes are all basically the same thing
* Functions can be sent arguments from the left side, right side or both (inputLeft.function: inputRight), which is used for class functions but can also allow you to define functions for any type that act like class functions
* Program control is done with operators instead of keywords (? instead of if)

## Current State
What I have currently working:
* A few primitive data types (Bool Int and Dub)
* some operators (+, -, :, =, >, <, etc.)
* The if statement (no then yet because tuples are not ready)
* The while loop
* the print function


	a: 7.6              # init a as a Dub
	b: Int              # init b as an Int with default value 0
	b: (Int: a)-(b-3    # convert a to an Int (truncate) and add it to b minus 3

	b<12?               # if b is less then 12 (true)
	    print: b        # print 10

	a>5?                # if a is greater then 5 (false)
	    print: a        # condition failed so don't execute This

	print               # print blank line

	i: 0                # init i as Int to 0
	i<24@               # while i is less then 24
	(                   # series of statements enclosed in parentheses (not curly braces)
	    print: i        # print i
	    i: i+1          # increment i
	)                   # when done, the numbers 0-23 will be printed, each on a new line

And that is just about a complete demonstration of currently implemented features.

## What's next
Right I'm working primarily on structuring the parser and the interpreter. Functions are also a big feature that I'd like to get out soon. Strings will likely come slightly later, but they are also very important. When I have all these things, the language will actually be usable. Then it will just be a matter of making it better.
