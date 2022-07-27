# NOTES.md

Stuff to remember about the project.

- Remember not to extraneous information between table or list markers and their
related markers, (like in the case of troff, .br being added), as this will mess
up the strict rules about table and lists. If our table looks like this before it
is processed by the writer:

```
\T
.br
\S ;
.br
...
```

Then it will error because the `\T` epects a `\S` immediately after. This is only
really a concern in troff, but either way, this is usually avoided by having blocks
like descriptions and notes be written with the `add_breaks` function, which will
not add .br's in between each line. All you must do is verify that the block you
are about to write is correct by calling the publuc function `writer_validate_string`.
As for tables or lists that may be written by the postprocessor, well, just do not
add .br's! That being said, make sure you call the validation function before the
call to `add_breaks`.
