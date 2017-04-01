# Control Flow
In Pinecone, control flow is done with symbols instead of keywords. There are two symbols used for control flow, `?` and `@`. `|` is used in conjunction with one of them.

## If/Then

`?` is used for ifs. You may be familiar with the ternary operator in C like languages. In Pinecone `?` can be used in much the same way (the only difference is `|` is used instead of `:`), but `?` is also used for all if statements with `|` being the 'else' operator. The body of an if statement is usually enclosed in parentheses, but this is optional if there is only one line in it. For 'else if', follow an else with another if statement. Here is an example:

```
a: tru
b: 8

a ?
    print: 1

b<4 ?
    print: 2
|
    print: 3

a && b<7 ?
(
    print: 4
)
| b=8 ?
(
    print: 5
)|(
    print: 6
)
```

The output of that code is

```
> 1
> 3
> 5
```

## loops

`@` is the loop operator. It is used for while and for loops.

A simple while loop is below:

```
i: 8

i>=0 @
(
    print: i
    i: i-1
)
```

Like if, the perenthesis would be optional if there was only one statement in the body.

For loops are are based on C for loops, in that the header consits of 3 expressions, the first sets it up, the second checks if to continue each iteration and the third increments a vairable. The expressions are seporated with `|`. Here is an example:

```
i: 0 | i<12 | i: i+1 @
(
    print: i
)
```

The output of this is the numbers 0-11.

[index](index.md) | [next: Strings and Input ->](3_strings_and_input.md)

