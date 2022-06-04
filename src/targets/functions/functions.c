/*
 * This file is the 'entry point' to the function documentation generator.
 * This contains
*/

#include "functions.h"

void docgen_functions_format(struct DocgenArguments arguments,
                            struct DocgenFunction function) {
    if(strcmp(arguments.format, "manpage") == 0)
        docgen_functions_manpage(arguments, function);
    else
        fprintf(stderr, "docgen: unknown format for functions '%s'\n", arguments.format);
}

void docgen_functions_generate(struct DocgenArguments arguments, FILE *file) {
    int index = 0;
    struct LibmatchCursor cursor = libmatch_cursor_from_stream(file);
    struct DocgenFunctions *functions = NULL;

    /* Comment junk */
    const char *comment_start = docgen_get_comment_start(arguments);
    const char *comment_end = docgen_get_comment_end(arguments);

    functions = docgen_extract_functions(&cursor, comment_start, comment_end);

    for(index = 0; index < carray_length(functions); index++) {
        struct DocgenFunction function = functions->contents[index];

        /* Generate this function manual */
        docgen_functions_manpage(arguments, function);
    }

    libmatch_cursor_free(&cursor);
    docgen_extract_functions_free(functions);
}
