# README.md

This file is intended to act as a guide on how to add new parsers and
backends. This does not include how to *write* either, but rather, what
parts of the program to change once you have written them to make the
program understand them.

Parsers will take an input file, as well as the parsed command line arguments,
and parse the input file for any tokens it can generate manuals for. When one
is found, it will parse the comment, and invoke its respective the backend
selector to convert the extracted token into the correct format.

Parsers and backends for a certain target / token are stored in the same parent
directory, `src/targets`. This directory contains all the targets that docgen
knows how to parse. For example, `src/targets/functions` is where the function
parsers and backends are stored. This is a general file layout:

```
src/
  targets/
    functions/
      functions.c /* Contains the parsing functions for a function */
      functions.h /* Contains all structure and function declarations for the parser and backends */
      backends/   /* The different types of formats a function target can be converted to */
        manpage.c   
```

When adding your own parsers and backends, please follow this directory
structure. It makes the project much easier to maintain.

# Source Code Changes

## SCC 1
When you are finished writing the parsers and backends, you also must make some
minor changes to the source code. Currently, docgen has no module system to
automatically generate the source code needed (and probably never will), so the
logic must be done by hand.

First, go to the root of the `src` directory tree, and open `docgen.h`. At the
line labeled with the comment `Categories to parse`, add a new enumeration to
the end of the existing list that represents the category the parsers and back-
ends are used for. The digit assigned to it should be +1 of the last enumeration
in the list.

The signature of the function that parses your token should also be put at the
bottom of this header.

## SCC 2
Next, open the `arguments.c` file, and find the `main_enumerate` function. At
the end of the chain of `if` blocks, add a new `if` statement that will compare
the category the user has chosen at the command line to the token you just made
parsers and backends for. Return the constant defined in *SCC 1*.

## SCC 3
Finally, open `main.c`. Go down to the `switch` statement labeled `Invoke the
correct category generator`. In this switch statement, add a new case that uses
the constant you defined in *SCC 1*. Inside of this case, invoke the function
you declared in at the bottom of the header in *SCC 1* with the arguments and
the file to read from.

# Conclusion
Once you have done all of this, your the program should compile and you should
be able to access your generation backend through `docgen`. Feel free to let me
know about any new ones you add so I can add them to the base program.

# Folder structure
`./doc` is the directory for documentation
`./scripts` is the directory for scripts used by the project
`./src` is the directory containing the source code
`./templates` is the directory that contains M4 templates
`./tests` is the directory that contains program tests

`./src/common` is the directory for common functions used by the extractors
`./src/extractors` is the directory for code that parses source code tokens.
`./src/targets` is the directory for documentation backends, and for parsers that are not for source code tokens (like categories and main pages)

## Rationale For Separation of `extractors` and `targets`
Put simply, `./src/extractors` is for code that has a likely chance of being
used by other kinds of documentation backends. For example, any number of the
documentation backends may benefit from being able to extract all of the
functions from a file, while it is unlikely that backends may find it useful to
be able to extract category and main page information.



