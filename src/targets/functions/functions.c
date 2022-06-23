/*
 * C-Ware License
 * 
 * Copyright (c) 2022, C-Ware
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 
 * 3. Redistributions of modified source code must append a copyright notice in
 *    the form of 'Copyright <YEAR> <NAME>' to each modified source file's
 *    copyright notice, and the standalone license file if one exists.
 * 
 * A "redistribution" can be constituted as any version of the source code
 * that is intended to comprise some other derivative work of this code. A
 * fork created for the purpose of contributing to any version of the source
 * does not constitute a truly "derivative work" and does not require listing.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/*
 * This file is the 'entry point' to the function documentation generator.
*/

#include "functions.h"

void docgen_functions_format(struct DocgenArguments arguments,
                            struct DocgenFunction function) {
    if(strcmp(arguments.format, "manpage") == 0)
        docgen_functions_manpage(arguments, function);
    else if(strcmp(arguments.format, "markdown") == 0)
        docgen_functions_markdown(arguments, function);
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
        docgen_functions_format(arguments, function);
    }

    libmatch_cursor_free(&cursor);
    docgen_extract_functions_free(functions);
}
