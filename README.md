# Lox Bytecode Compiler

Lox is an Object Oriented language designed by Bob Nystrom in his book [Crafting Interpreters](https://craftinginterpreters.com/) to teach design principals behind language design and data structures used to design compilers.

---

This version of lox closely follows Bob's book and was aimed at creating an efficient bytecode interpreter to compile lox. The first half of the book demonstrated how a tree-walk interpreter works. His implementation was written in java, known as jlox, you can check out my implementation of the tree-walk interpreter, known as pylox, [here](https://github.com/galaxyproduction/pyLox).

## Requirments
* cmake: version <= 3.20

## To run
You can run a repl with:

`$ ./build/apps/main`

<br>
Or you can run a lox file with: 

`$ ./build/apps/main nameOfFile` 
<br>

## Run a test project
I have included two test projects to see lox running.
<br>
To see an example of inheritance in Lox run:

`$ ./build/apps/main ./TestProjects/Lox_Inheritance.lox`

<br>
To see an example of Lox classes run:

`$ ./build/apps/main ./TestProjects/Lox_LinkedLists.lox`

## Author
* Hunter Wilkins
* [hunter.wilkins.dev](https://hunterwilkins.dev)
* hunter.wilkins2@gmail.com