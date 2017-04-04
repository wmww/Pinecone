# The Pinecone Programming Language
**_Built from the ground up to be fast, concise and intuitive._**

Pinecone is a new programming language. Its goal is to combine the simplicity of a dynamic language with the performance of a compiled one. It is under rapid development, but most of the core features are ready.

__If you want to program in Pinecone now, see the [tutorials](tutorials/index.md) for how to get started.__

__For updates, discussion and help, take a look at the Pinecone subreddit [/r/PineconeLang](https://www.reddit.com/r/PineconeLang/)__

## About
Pinecone is a brand new, easy to learn, general purpose, multi-paradigm, high performance programming language designed by Sophie Winter. Work on the language began on October 4th, 2016. The language is written from scratch (it includes an integrated lexer and parser). Right now Pinecone can be transpiled to C++ or interpreted, and compiling with LLVM is planned for the future.

## Example
here is the common demo program FizzBuzz written in Pinecone. It prints the numbers from 1 to 20, but it prints "Fizz" if the number is divisible by 3, "Buzz" if it is divisible by 5 and "FizzBuzz" if it is divisible by both. You can find more samples in the examples directory or the tutorials.

```
# FizzBuzz

# call the function defined below
fizzBuzz: 1, 20

# define the FizzBuzz function
fizzBuzz :: {start: Int, end: Int}: (

	# loop i from start to end
	i: in.start | i <= in.end | i: i+1 @ (

		# use conditionals to print the right thing

		i % 3 = 0 && i % 5 = 0 ?
			print: "FizzBuzz"
		|
		i % 3 = 0 ?
			print: "Fizz"
		|
		i % 5 = 0 ?
			print: "Buzz"
		|
			print: i
	)
)
```

## Why?
This is probably the most common reaction to hearing about a new language. I realize that there are a __lot__ of programming languages, and that the reason for that is that there are so many assholes like me who keep making them. I do truly think, though, that Pinecone fills a previously empty niche.

Pinecone aims to have similar capabilities to modern object oriented compiled languages such as C++, Swift and Rust. It's primary attraction is the simplicity and consistency of it's syntax. Here are some examples of how Pinecone is different from other popular languages:

* Variable creation is implicit, just set a variable and it is created.
* Variables are statically typed, but type deduction is automatic.
* Calling a function that takes no arguments is the same syntax as accessing a variable (just writing it's name).
* Calling a function that takes one argument is the same syntax as setting or creating a variable (`funcOrVar: input`).
* Calling a function that takes multiple arguments is the same syntax as setting or creating a tuple (`funcOrTuple: input1, input2`).
* Because of above three, switching a class property between a variable and a getter/setter does not require refactoring clients of the class.
* White space is ignored _and_ semicolons are not necessary.
* `:` is used for assignment, which leaves `=` free for comparison, rather than the often confusing `==`.
* Program control is done with operators instead of keywords (`?` instead of `if`).
* Better block comment syntax (`//` to start and `\\` to end).
* No unnecessary distinctions between tuples, structs and classes.
* Functions can be sent arguments from the left side, right side or both (`inputLeft.function: inputRight`), which makes creating and extending classes possible, and can be used in other situations.
* The `Whatev` type (currently under development) allows a single function to operate on different types, this works similarly templates and generics, but with less syntax overhead.

## Compatibility
Pinecone has been successfully compiled on Linux, MacOS and Windows. Development is primary done on Linux, so other platforms may occasionally have issues. If you run into any problems, open up an issue on GitHub and I'll look at it as soon as I can. Pincone currently requires zero external dependencies (and the only one that will likely be added is LLVM). You will need a C++11 compiler to build it, and building is easier with make and GCC.

## Current State
The features that are currently implemented are as follows:

* Primitive data types `Bool`, `Int` and `Dub`
* All the operators you would expect (`+`, `*`, `%`, `:`, `=`, `>`, `<=`, `&&`, etc.)
* Single and multi line comments
* Flow control (if, if/else, while loop, for loop)
* Constants
* Data structs
* Tuples
* Int arrays
* Functions
* Strings and various String operations
* User input
* Running system commands
* Interpreter for rapid development and simplicity
* Transpiler to C++ for max performance

the following features are coming soon:

* `Whatev` type (equivalent to templates and generics)
* Arrays of any type (Whatev support needed)
* Pass-by-reference
* Proper classes (pass-by-reference needed)
* Operator overloading

## Contributing
I have not yet added enough documentation to the language internals to make it practical for others to contribute to the language itself. If you are interested in adding a specific feature or just helping out, post in the [subreddit](https://www.reddit.com/r/PineconeLang/) or direct message me on [reddit](www.reddit.com/u/william01110111/) or [twitter](https://twitter.com/PineconeLang). Fixes and improvements to the readmes and tutorials are always welcome.


