# The Pinecone Programming Language
**_Built from the ground up to be fast, concise and intuitive._**

Pinecone is a new programming language. Its goal is to combine the simplicity of a dynamic language with the performance of a compiled one. It is under rapid development, but most of the core features are ready.

__If you want to program in Pinecone now, see the [tutorials directory](tutorials) for how to get started.__

__For updates, discussion and help, take a look at the Pinecone subreddit: [/r/PineconeLang](https://www.reddit.com/r/PineconeLang/)__


## About
Pinecone is a brand new, easy to learn, general purpose, multi-paradigm, high performence programming language created by William W Wold. Work on the language began on October 4th, 2016. Right now effort is focused on the Pinecone interpreter, but development will soon begin on a compiler using LLVM. The language is written from scratch (it includes an integrated lexer, parser and interpreter).

## Why?
This is probably the most common reaction to hearing about a new language. I realize that there are a __lot__ of programming languages, and that the reason for that is that there are so many assholes like me who keep making them. I do truly think, though, that Pinecoe fills a previously empty niche.

Pinecone will be similar in capabilities to modern object oriented compiled languages such as C++, Swift and Rust. It's primary attraction is the simplicity and consistency of it's syntax. Here are some examples of how Pinecone is different from other popular languages:

* Variable creation is implicit, just set a variable and it is created.
* Variables are statically typed, but type deduction is automatic.
* Calling a function that takes no arguments is the same syntax as accessing a variable (just writing it's name).
* Calling a function that takes one argument is the same syntax as setting or creating a variable (`funcOrVar: input`).
* Calling a function that takes multiple arguments is the same syntax as setting or creating a tuple (`funcOrTuple: input1, input2`).
* White space is ignored _and_ semicolons are not necessary
* `:` is used for assignment, which leaves `=` free for comparison, rather then the often confusing `==`.
* Tuples, structs and classes are all basically the same thing
* Functions can be sent arguments from the left side, right side or both (`inputLeft.function: inputRight`), which is used for class methods but can also allow you to define functions for any type (even primitive).
* Program control is done with operators instead of keywords (`?` instead of `if`)

## Current State
The features that are currently implemented are as follows:

* Primitive data types `Bool`, `Int` and `Dub`
* All the operators you would expect (`+`, `*`, `%`, `:`, `=`, `>`, `<=`, `&&`, etc.)
* Single and multi line comments
* Flow control (if, if/else, while loop, for loop)
* Constants that can be defined in any order
* Data structs
* Tuples
* Int arrays
* Functions that can take left and right inputs, and can return values
* Strings

the following features are coming soon:'

* Pass-by-reference
* Proper classes (pass-by-reference needed)
* Arrays for any type
* Operator overloading
* Transpiling to C++, or compiling using LLVM

here is an example of a very simple Pinecone program. It prints out all the fibonacci numbers that are less then 100 and even.
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
