
## Repl
`repl.sh` is a BASH script that creates a Pinecone repl. It supports calling functions and variables, but does not properly handle all symbols (such as `?`) and does not work for multi line expressions.

To run the repl (after pinecone has been installed):

 * Make the `repl.sh` excitable: `chmod +x repl/repl.sh`  
 * Run with `./repl/repl.sh`
 * Optional: Create an alias. Add `alias pinecone=".../path/to/pinecone/repl/repl.sh"` to your `.rc` file (`~/.bashrc`, `~/.zshrc`, ect.).

Commands:

 * `.clear`: Clears the repl.
 * `.read` : Prints what is in the repl.
 * `.exit` : Exits the repl (same as ^C).
