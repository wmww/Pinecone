# Basic Concepts of the Pinecone Language

Most Beginner tutorials start with a hello world program. Don't get me wrong, I would love to, but since I have not yet implemented strings, it would be a mess of nonstandard functions and ASCII codes. Instead, lets start with a program that prints out something far more important.
```
print: 42
```
That's all you need in your file. No includes or boilerplate code of any sort. Just save that as a normal text file with the extension `.pncn` and run it.

## Primitive Types

42 is an example of an __Int__, which is the same as an int in C++. It can hold positive and negative whole numbers. The other two primitive data types are Dub and Bool.

__Dub__ is the same as a C++ double. It can hold decimal numbers. a Dub literal either ends with a d (ex: `47d`) or has a decimal point (ex: `47.0`). _NOTE: while a Dub literal can start with a decimal point, it can not end with one (ex: `.5` is valid but `5.` is not )_.

A __Bool__ can only be `tru` or `fls`. If you think it should be `true` and `false` instead, you can email your complaints to williamwold@idontgiveafuck.com.

## Calling Functions

`print: 42` is and example of a function call. __print__ is a standard function that prints what it is sent, followed by an newline. the ':' indicates that you want to sent input to print and whatever follows is the input.

Functions can only take input of the exact type they are supposed to take (implicit casting will eventually be but is not yet implemented). Functions can, however be overloaded. This means there are multiple functions with the same name that are diferentiated by what type their input is.

print is an overloaded function, so you can also do `print: 42.1`, or even just `print` and it will compile (the latter takes no input and so it will just print an empty line).

Functions can also take input from the left, but we will get to that later.

## Variables

A __variable__ is a place you can store a value. Every variable has a type, but types are deduced implicitly. To create, change and use a variable, simply do the following:
```
myVarName: 88
myVarName: 12
print: myVarName
```
`myVarName` can be any serias of letters, digits and underscores, as long as it doesn't start with a number. Variable names are case sensitive.

As you can see, creating a variable, setting a variable and calling a function with all look the same. This is a very important feature for the language, as it makes refactoring variables into calculations much easier.

## Comments

Comments are parts of your program that the compiler doesn't look at, so you can write notes and whatnot. In Pinecone, singel-line comments start with a `#`. Multi-line comments start with `\\` and end with `//`.

## Explicit Type Conversion

Sometimes, you might want to send a value to a variable or function of a different type. In the future, this will just work, but for now, you must convert it explicity. To do this, call the target type's name like a function. For example:
```
myDub: 12.7
myDub: 32       # won't work because 32 is an Int
myDub: Dub: 32  # explicit conversion
```
