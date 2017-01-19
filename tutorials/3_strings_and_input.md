# Strings and Input

In Pinecone, like in many langauges, a string is a series of characters of any length. To make a string, simply suround some text in double quotes:
```
myString: "this is a string!!!"
```

## Operators and Functions

Like other types, there are various operators and functions that can be applied to strings. `=` checks if two strings are exactly the same and `+` combines two strings. `stringName.len` returns the length of that string. If you call the function ascii on an int (ex: `68.ascii`), It will return a string one character long containing the ascii value of that int. To get a substring, call the method `stringName.sub: start, end`. I use start and end instead of start and length because start and length is stupid.

## String input

You can get a string as an input from the user. To do this, call `String.input`. This will block execution until they type sonething in and press enter, at which time the finction will return a string. If you want to give the user a prompt, call the function like this: `"please enter some input: ".input`.

[index](index.md) | [next: Structures and Functions ->](4_structures_and_functions.md)
