# Setting up your environment

Disclamer: these steps will probably work on Linux, maybe even on OSX. As for Windows, I have no idea. Good luck.

1. Open a terminal and navigate to where you want the Pinecone folder to go
2. Run the command `git clone https://github.com/william01110111/Pinecone.git`. This will download the Pinecone source code
3. Move into the Pinecone directory
4. run the command `make`. If you get a _command not found_ error, you may need to install make or g++.
5. If all goes well, after a few seconds you should end up with a Pinecone executable in your current directory called `pinecone`.
6. Run it with the command `./pinecone path/to/pinecone/source.pn`. If you don't have any Pinecone source yet, test it with `./pinecone examples/hello_world.pncn`
7. If it works, you can now move on to the "basic concepts" tutorial

[index](index.md) | [next: Basic Concepts ->](1_basic_concepts.md)

