# Basic Concepts

The tutorials for most programming languages start with a hello world program, but Pinecone isn't most programming languages. Instead, lets start with a program that prints out something far more important.

```
print: 42
```

That's all you need in your file. No includes or boilerplate code of any sort. Just save that as a normal text file with the extension `.pn` and run it.

## Primitive Types

42 is an example of an __Int__, which is the same as an int in C++. It can hold positive and negative whole numbers. The other two primitive data types are Dub and Bool.

__Dub__ is the same as a C++ double. It can hold decimal numbers. a Dub literal either ends with a d (ex: `47d`) or has a decimal point (ex: `47.0`). _NOTE: while a Dub literal can start with a decimal point, it can not end with one (ex: `.5` is valid but `5.` is not )_.

A __Bool__ can only be `tru` or `fls`. If you think it should be `true` and `false` instead, you can email your complaints to williamwold@idontgiveafuck.com.

A __String__ isn't really a primitive data type, but it is very common so I'll include it here anyway. It is a series of letters, numbers or other characters of any length. You make a string by surrounding text in double quotes.

## Operators

In general, operators in Pinecone work the same as in any other language. It has all the ones you would expect with sensible order of operations. The following are the only major differences between operators in Pinecone and C-style langauges:
* The assignment operator is `:` instead of `=`.
* The equality operator is `=` instead of `==`.
* There are no bitwise operators, they may be implemented at some point.
* The short circuit aspect of boolean operators doesn't work yet, so `fls && functionReturningBool` will execute functionReturningBool. This will be fixed soon.

## Calling Functions

`print: 42` is an example of a function call. __print__ is a standard function that prints what it is sent, followed by an newline. the ':' indicates that you want to sent input to print and whatever follows is the input.

Functions can only take input of the exact type they are supposed to take (implicit casting will eventually be but is not yet implemented). Functions can, however be overloaded. This means there are multiple functions with the same name that are differentiated by what type their input is.

print is an overloaded function, so you can also do `print: 42.1`, or even just `print` and it will compile (the latter takes no input and so it will just print an empty line).

Functions can also take input from the left, but we will get to that later.

## Variables

A __variable__ is a place you can store a value. Every variable has a type, but types are deduced implicitly. To create, change and use a variable, simply do the following:

```
myVarName: 88
myVarName: 12
print: myVarName
```

`myVarName` can be any series of letters, digits and underscores, as long as it doesn't start with a number. Variable names are case sensitive.

As you can see, creating a variable, setting a variable and calling a function with all look the same. This is a very important feature for the language, as it makes refactoring variables into calculations much easier.

## Tuples

A Tuple can be thought of as an ad hoc data structure. To construct one you simple combine several expressions with commas. The names of the elements of a tuple are `a`, `b`, `c`, etc. Elements can be accesed with the `.` operator. Here is an example:

```
myTuple: 6, 2.9, fls
print: myTuple.c
print: myTuple.a
```

The output of this will be

```
> fls
> 6
```

Tuple elements are supposed to be mutable, but this functionality is currently broken.

## Constants

A __constant__ is a value that is determined at compile time. Constants are created with the constants assignment operator `::`. You can declare constants above or below where you use them. Trying to set a constants more then once will result in a compile time error. Here is an example of a simple use of constants:

```
a :: 3
print: b
b :: a+c
c :: 2
```

This will compile correctly and print 5.

## Comments

Comments are parts of your program that the compiler doesn't look at, so you can write notes and whatnot. In Pinecone, single-line comments start with a `#`. Multi-line comments start with `//` and end with `\\`.

## Explicit Type Conversion

Sometimes, you might want to send a value to a variable or function of a different type. In the future, this will just work, but for now, you must convert it explicitly. To do this, call the target type's name like a function. For example:

```
myDub: 12.7
myDub: 32       # won't work because 32 is an Int
myDub: Dub: 32  # explicit conversion
```

[index](index.md) | [next: Control Flow ->](2_control_flow.md)
