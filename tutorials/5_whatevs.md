# Whatevs

## The Problem
Sometimes explicitly declaring a type is inconvenient. You don't want a function to always take an Int, String or any other specific type. You just want it to take whatev. Thats why Pinecone has the Whatev type.

## How it Works
Whatevs in Pinecone work very similar to templates and generics in other languages. You declare a function that takes a Whatev, and then the compiler makes different versions of that function for each type you try to send into it. Note that Whatevs do not enable dynamic typing at runtime. You can not make a Whatev variable except as an argument to a function.

## Using Whatevs
The syntax is amazingly simple. Whatev is just a type, so you can use it like so:
```
# take any type, convert it to a string and return it wrapped in parenthesis
putInPeren :: {Whatev} -> {Whatev}:
(
	"(" + in.String + ")"
)

print: putInPeren: 8
print: putInPeren: tru
```
The output of this code is
```
(8)
(tru)
```
Note that all types you use this function with must have an overload of the .String function defined, else you'll get a compile time error.

## Whatevs in Structs
Whatevs can be used in structs. here is an example:
```
# define the struct

MyWhatevStruct ::
{
	foo: Int
	bar: Whatev
}

# define the constructor

makeMyWhatevStruct :: {a: Int, b: Whatev} -> {MyWhatevStruct}:
(
	in.a, in.b
)

# define the print function for makeMyWhatevStruct

print :: {MyWhatevStruct}:
(
	print: in.foo.String + ", " + in.bar.String
)

# use the struct

a: makeMyWhatevStruct: 8, 9.3
b: makeMyWhatevStruct: 2, fls

print: a
print: b
```
The output of this is
```
8, 9.3
2, fls
```

[index](index.md) | [next: Transpiling to C++ ->](6_transpiling_to_cpp.md)
