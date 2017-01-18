# Temporary Hacks

Pinecone is still under rapid development, and there are a lot of features that are broken or not yet implemented. This tutorial explains what is coming and what you can use until then. many of the functions described here may be depricated/removed as soon as better replacements are ready.

## Pass-by-reference
There is not yet any pass-by-reference in Pinecone (with the exception of Int arrays and strings (though the latter is immutable)). Without pass-by-reference, you can't really have classes because functions can not modify the object they are called on.

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
As of v0.3.0, strings work and the printc function is deprecated. It will be removed soon.

## Input
Getting string input works great (see string tutorial). Int input is not yet standardized, but you can call the function `inputInt` to get a number from the user.

## imports
Importing works similar to how it does in Python. It simply dumps another file into your current file. Right now, the import syntx is
```
==> "path/to/target/file/from/current/file.pncn"
```
For example, to import the examples/letters file into other/pinecone, you write
```
==> "../examples/letters.pncn"
```
I may keep this syntax, but I'm not sure so I'm keeping it in this tutorial.
