# Temporary Hacks

Pinecone is still under rapid development, and there are a lot of features that are broken or not yet implemented. This tutorial explains what is coming and what you can use until then. many of the functions described here may be depricated/removed as soon as better replacements are ready.

## Pass-by-reference
There is not yet any pass-by-reference in Pinecone (with the exception of Int arrays). Without pass-by-reference, you can't really have classes because functions can not modify the object they are called on.

## Global Variables
Globals are broken. What I mean by that is using a variable in a function that was created outside of that function does not work. It will compile, it just will crash or give rubbish output. Don't do it.

## Arrays
I have some very cool ideas for array syntax, but it is not ready yet. For now, you can have an array of ints with the built in class IntArray. Here is an example of how to use it:
```
myArray: IntArray: 6                # 6 is the length of the array

i: 0 | i<myArray.size | i: i+1 @
(
    myArray.set: i, i*2+1           # set index i to value i*2+1
)

print: myArray.get: 2               # get the element at index 2

```
The output is `5`.

## Strings and Text
Strings (and even chars) are not yet implemented. You can however print text to the screen with the `printc` function. You send it an Int and it prints the ASCII character of that Int without a newline afterwords.
