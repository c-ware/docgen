# CHANGELOG.md

VERSION: 2.0.0
DATE: July 25, 2022

This update is a fundamental change in how docgen works, and so it cannot really
be summarized by a simple list of changes. Originally, docgen worked by having
a functions which generated each type of token (macro functions, functions,
projects, etc) for each target (manpages, markdown, etc).

This was an absolute NIGHTMARE to maintain, and after struggling to make whitespace
appear in correct locations (yes, really) in manuals, I decided to make a fundamental
change in docgen's structure.

The extractors would remain the same, but the actual generation would be changed.
Rather than have individual functions perform the formatting, and writing, we now
have a sort of "pipeline" where different stages of generation are done. The pipeline
looks like this:

Extractor functions -> Postprocessor data generation -> Postprocessor -> Writer

The extractor functions are fundamentally the same as they were in the original docgen.
Minimal changes have been made to them except for extracting more settings, but the
introduction of a 'post processor' is what makes this such a massive change.

The postprocessor is the thing that assembles input into a single string. It makes sure
that the the format-specific quirks and formatting requirements are where they should be.
It also makes sure the whitespace is correctly formatted!

The postprocessor will however need input given to it, which is gathered by a generator
function. Each generator function is given a 'thing' to generate data for. For example,
a macro function. This macro function is assembled from a fully parsed structure from the
extractor for macros. The generator will grab all the requested embeds in the macro function,
invoke formatter functions to transform each embed of each type into a list of CStrings, and
make sure all the data required for the final product is assembled for the postprocessor.

As for the writer, it handles format-agnostic things like expanding tables, lists, and performing
formatting like bolding and italicizing text. It does this as it writes into the file, as to not
use more memory than this program already uses.

The writer uses certain markers to tell the writer to perform certainn formatting techniques.
This could be something like bolding, italics, tables, lists, etc.
\E    list element
\S    table separator
\L    start a list
\B    start or end bold
\I    start or end italics
\T    start or end a table

Note, however, that docgen does NOT support nested formatters, like for example
**foo**, or **__foo__** in Markdown.

That is pretty much it.
All this just to make whitespace appear correctly. Jesus christ.

