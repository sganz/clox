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

## Orig Authors
* Hunter Wilkins for CLOX work
* [hunter.wilkins.dev](https://hunterwilkins.dev)
* hunter.wilkins2@gmail.com
* Caleb Schoepp for NQQ work - https://github.com/calebschoepp/nqq

## HACKING PROGRESS
Adding code from the NQQ version of the CLOX interpreter. The NQQ versions does not support objects
but does support lists and maps and a host of other nice features. Need to get code integrated with CLOX
that allows objects.

### Current progress
* 10/16/2021
    * Integrating files up to now scanner (from Header List of lox)
    * Need to resove string type (raw/basic/template etc.)
* 10/17/2021
    * More stuff, commit to github, will remove stuff as needed. 
    * botched up git a bit, and tried to do a pull request to orig clox. Trying to unconnect
    from the master on the orig project but need a ticket to do that or paid version of git.
    * Will for sure need to change many of the ObjString to Value since that is used by internal 
    new NQQ functions for Hash Table (table.c and others). This allows the hash key to not just be an
    int but any object type as needed. So can hash a string, int, etc. Good call NQQ