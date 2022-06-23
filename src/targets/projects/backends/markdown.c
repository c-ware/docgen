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

#include "../projects.h"

#define write_inclusions(array)                                            \
do {                                                                       \
    int __INCLUSION_INDEX = 0;                                             \
                                                                           \
    while(__INCLUSION_INDEX < carray_length(array)) {                      \
        struct Inclusion inclusion = (array)->contents[__INCLUSION_INDEX]; \
                                                                           \
        if(arguments.md_mono == 1)                                         \
            fprintf(location, "%c", '`');                                  \
                                                                           \
        if(inclusion.type == DOCGEN_INCLUSION_LOCAL) {                     \
            fprintf(location, "#include \"%s\"", inclusion.path);          \
        } else if(inclusion.type == DOCGEN_INCLUSION_SYSTEM) {             \
            fprintf(location, "#include <%s>", inclusion.path);            \
        }                                                                  \
                                                                           \
        if(arguments.md_mono == 1)                                         \
            fprintf(location, "%c", '`');                                  \
                                                                           \
        fprintf(location, "%c", '\n');                                     \
                                                                           \
        __INCLUSION_INDEX++;                                               \
    }                                                                      \
} while(0)

static void head(FILE *location, struct DocgenArguments arguments, struct DocgenProject project) {
    fprintf(location, "# %s.md\n\n", project.name);
    fprintf(location, "%s", "### NAME\n");
    fprintf(location, "%s - %s\n\n", project.name, project.brief);
}

static void synopsis(FILE *location, struct DocgenArguments arguments, struct DocgenProject project) {
    int index = 0;

    fprintf(location, "%s", "### SYNOPSIS\n");

    write_inclusions(arguments.inclusions);

    fprintf(location, "%c", '\n');


    fprintf(location, "%s", "\n\n");
}

static void description(FILE *location, struct DocgenArguments arguments, struct DocgenProject project) {
    fprintf(location, "%s", "### DESCRIPTION\n");
    fprintf(location, "%s", project.description);
    fprintf(location, "%s", "\n");
}

static void see_also(FILE *location, struct DocgenArguments arguments, struct DocgenProject project) {
    int index = 0;

    fprintf(location, "%s", "### SEE ALSO\n");

    if(carray_length(project.references) == 0) {
        fprintf(location, "%s", "This project has no references.\n");
        return;
    }

    while(index < carray_length(project.references)) {
        struct Reference reference = project.references->contents[index];

        fprintf(location, "%s(%s)", reference.manual, reference.section);

        /* Add a comma */
        if(index < (carray_length(project.references) - 1))
            fprintf(location, "%s", ", ");

        index++;
    }

    fprintf(location, "%c", '\n');
}

void docgen_project_markdown(struct DocgenArguments arguments, struct LibmatchCursor cursor, struct DocgenProject project) {
    int index = 0;
    FILE *location = NULL;
    char file_path[LIBPATH_MAX_PATH + 1];

    /* Open the file */
    docgen_create_file_path(arguments, project.name, file_path, LIBPATH_MAX_PATH);
    libpath_join_path(file_path, LIBPATH_MAX_PATH, "./doc/", project.name,
                      ".md", NULL);
    location = fopen(file_path, "w");

    /* Dump parts of the Markdown */
    head(location, arguments, project);
    synopsis(location, arguments, project);
    description(location, arguments, project);
    see_also(location, arguments, project);
}
