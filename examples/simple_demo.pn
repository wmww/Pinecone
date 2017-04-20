
//
This file demonstrates the basic syntax constructs of Pinecone

to run, simply run command
path/to/pinecone/executable simple_demo.pn
\\

# that was a multi line comment. this is a single line comment

# create a variable
a: 12 # deduces the type as Int

# print the value in the variable
print: a

print # print an empty line

print: "if/then"

b: 7

a = 9 ?
(
	print: 1
)|(
	print: 2
)

b>3 ?
(
	print: 3
)

print


print: "while loop"

i: 12

i>3 @
(
	print: i
	i: i-1
)

print


print: "for loop"

i: 0 | i<10 | i: i+1 @
(
	print: i
)

print


print: "functions"

print: func: 7

func :: {Int} -> {Dub}:
(
	(Dub: in)/2.0
)

print


print: "Fibonacci"

b: 0
a: 1

a<100 @
(
	print: a
	tmp: a
	a: a+b
	b: tmp
)

print


print: "Recursive Fibonacci"

fib :: {Int} -> {Int}: (
	out: Int
	
	in <= 1 ?
		out: 1
	|
		out: (fib: in - 1) + (fib: in - 2)
	
	out
)

print: fib: 8

print

print: "compile time constants"
# these can be declared in any order

print: y

x :: 10
y :: x+z
z :: 4


