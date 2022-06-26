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
 * The backend for converting 'function' tokens into Markdown files
*/

#include <string.h>

#include "../functions.h"

#define write_inclusions(array)                                            \
do {                                                                       \
    int __INCLUSION_INDEX = 0;                                             \
                                                                           \
    while(__INCLUSION_INDEX < carray_length(array)) {                      \
        struct Inclusion inclusion = (array)->contents[__INCLUSION_INDEX]; \
                                                                           \
        if(inclusion.type == DOCGEN_INCLUSION_LOCAL) {                     \
            fprintf(location, "#include \"%s\"", inclusion.path);          \
        } else if(inclusion.type == DOCGEN_INCLUSION_SYSTEM) {             \
            fprintf(location, "#include <%s>", inclusion.path);            \
        }                                                                  \
                                                                           \
        fprintf(location, "%c", '\n');                                     \
                                                                           \
        __INCLUSION_INDEX++;                                               \
    }                                                                      \
} while(0)


static void head(FILE *location, struct DocgenArguments arguments, struct DocgenFunction function) {
    fprintf(location, "# %s.md\n\n", function.name);
    fprintf(location, "%s", "### NAME\n");
    fprintf(location, "%s - %s\n\n", function.name, function.brief);
}

static void synopsis(FILE *location, struct DocgenArguments arguments, struct DocgenFunction function) {
    int index = 0;

    fprintf(location, "%s", "### SYNOPSIS\n");

    write_inclusions(arguments.inclusions);
    write_inclusions(function.inclusions);

    fprintf(location, "%c", '\n');

    /* Write the function signature */
    fprintf(location, "`%s %s(", function.return_data.return_type,  function.name);

    /* Function parameters */
    for(index = 0; index < carray_length(function.parameters); index++) {
        struct DocgenFunctionParameter parameter = function.parameters->contents[index];

        /* Use backticks for generating a monospaced font for code. */
        if(strchr(parameter.type, '*') == NULL)
            fprintf(location, "%s %s", parameter.type, parameter.name);
        else
            fprintf(location, "%s%s", parameter.type, parameter.name);

        // Do not output a comma
        if(index == (carray_length(function.parameters) - 1))
            continue;
        
        fprintf(location, "%s", ", ");
    }

    fprintf(location, "%s", ")`\n\n");
}

static void description(FILE *location, struct DocgenArguments arguments, struct DocgenFunction function) {
    fprintf(location, "%s", "### DESCRIPTION\n");
    fprintf(location, "%s", function.description);
    fprintf(location, "%s", "\n");
}

static void return_value(FILE *location, struct DocgenArguments arguments, struct DocgenFunction function) {
    fprintf(location, "%s", "### RETURN VALUE\n");

    /* No return value, or no description provided */
    if(function.return_data.return_value[0] == '\0')
        fprintf(location, "%s", "This function has no return value.\n");
    else
        fprintf(location, "This function will return %s\n", function.return_data.return_value);

    fprintf(location, "%s", "\n");
}

static void notes(FILE *location, struct DocgenArguments arguments, struct DocgenFunction function) {
    fprintf(location, "%s", "### NOTES\n");

    /* No notes */
    if(function.notes[0] == '\0')
        fprintf(location, "%s", "This function has no notes.\n");
    else
        fprintf(location, "%s\n", function.notes);
}

static void examples(FILE *location, struct DocgenArguments arguments, struct DocgenFunction function) {
    fprintf(location, "%s", "### EXAMPLES\n");

    /* No examples */
    if(function.example[0] == '\0')
        fprintf(location, "%s", "This function has no examples.\n");
    else
        fprintf(location, "%s\n", function.example);
}

static void see_also(FILE *location, struct DocgenArguments arguments, struct DocgenFunction function) {
    int index = 0;

    fprintf(location, "%s", "### SEE ALSO\n");

    if(carray_length(function.references) == 0) {
        fprintf(location, "%s", "This function has no references.\n");
        return;
    }

    while(index < carray_length(function.references)) {
        struct Reference reference = function.references->contents[index];

        fprintf(location, "%s(%s)", reference.manual, reference.section);

        /* Add a comma */
        if(index < (carray_length(function.references) - 1))
            fprintf(location, "%s", ", ");

        index++;
    }

    fprintf(location, "%c", '\n');
}

void docgen_functions_markdown(struct DocgenArguments arguments, struct DocgenFunction function) {
    int index = 0;
    FILE *location = NULL;
    char file_path[LIBPATH_MAX_PATH + 1];

    /* Open the file */
    docgen_create_file_path(arguments, function.name, file_path, LIBPATH_MAX_PATH);
    libpath_join_path(file_path, LIBPATH_MAX_PATH, "./doc/", function.name,
                      ".md", NULL);
    location = fopen(file_path, "w");

    /* Dump parts of the Markdown */
    head(location, arguments, function);
    synopsis(location, arguments, function);
    description(location, arguments, function);
    return_value(location, arguments, function);
    notes(location, arguments, function);
    examples(location, arguments, function);
    see_also(location, arguments, function);

    fclose(location);
}
