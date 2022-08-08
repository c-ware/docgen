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
 * This is the entry point for the docgen compiler for C tags. It reads input
 * line by line and produces a target-independant representation of all of the
 * tags for the target to produce into documentation.
 *
 * A lot of error handling goes into this compiler, so here is the error 'tree'
 * where each error relies on the checks performed by all errors above it.
 *
 * - Newline at the end of the file
 *      - Each line starts with '@'
 *      - No empty lines (0 characters, \n\n)
 *      - All multiline tags are closed
 *          - All tags fit in the length requirements
 *              - All tags are recognized

*/

#include <string.h>
#include <stdlib.h>

#include "../../docgen.h"
#include "../../common/errors/errors.h"

#include "main.h"

#define SKIP_LINE_IF_TAG(location, tag)                                    \
do {                                                                       \
    if(strncmp(stdin_body.contents + cindex, (tag), strlen((tag))) == 0) { \
        cindex = CHAR_OFFSET(stdin_body.contents, newline) + 1;            \
                                                                           \
        continue;                                                          \
    }                                                                      \
while(0)

#define TOGGLE_MULTILINE_TAG(tag)                                        \
    if(strncmp(stdin_body.contents + cindex, (tag), strlen((tag))) == 0) \
        INVERT_BOOLEAN(in_multiline_tag)

/*
 * Produce an error message if the stdin body does not end with a
 * newline (0x10)
*/
void error_ends_without_newline(struct CString stdin_body) {
    VERIFY_CSTRING(stdin_body);

    /* No need to check if (stdin_body.length - 1) is in bounds,
     * since (length - 1) is < 0 only when length <= 0 */
    if(stdin_body.contents[stdin_body.length - 1] == '\n')
        return;

    fprintf(LIBERROR_STREAM, "%s", PROGRAM_NAME ": stdin does not end with a new line\n");
    exit(EXIT_MISSING_NEWLINE);
}

void error_line_doesnt_start_with_at(struct CString stdin_body) {
    int cindex = 0;
    int round = 0;

    VERIFY_CSTRING(stdin_body);

    /* Scan the start of each line. If it starts with a @, we
     * are good. If not, yell. Loudly. */
    while(cindex < stdin_body.length) {
        LIBERROR_OUT_OF_BOUNDS(cindex, stdin_body.length);

        /* This line is A-OK. Jump to the next one, since we can be
         * sure each line has a newline after it due to previous
         * checks. */
        if(stdin_body.contents[cindex] == '@') {
            char *newline = strchr(stdin_body.contents + cindex, '\n');

            LIBERROR_IS_NULL(newline);
            LIBERROR_IS_NOT_VALUE(*newline, '\n');
            LIBERROR_IS_NEGATIVE(CHAR_OFFSET(stdin_body.contents + cindex, newline));

            cindex = CHAR_OFFSET(stdin_body.contents, newline) + 1;

            continue;
        }

        fprintf(LIBERROR_STREAM, PROGRAM_NAME ": line %i does not start with an '@'\n",
                common_errors_get_line(stdin_body, cindex) + 1);
        exit(EXIT_LINE_DOESNT_START_WITH_AT);
    }
}

void error_empty_line(struct CString stdin_body) {
    int cindex = 0;

    VERIFY_CSTRING(stdin_body);

    /* Verify that each line has text on it */
    while(cindex < stdin_body.length) {
        LIBERROR_OUT_OF_BOUNDS(cindex, stdin_body.length);

        /* This line is A-OK. Jump to the next one, since we can be
         * sure each line has a newline after it due to previous
         * checks. */
        if(stdin_body.contents[cindex] != '\n') {
            char *newline = strchr(stdin_body.contents + cindex, '\n');

            LIBERROR_IS_NULL(newline);
            LIBERROR_IS_NOT_VALUE(*newline, '\n');
            LIBERROR_IS_NEGATIVE(CHAR_OFFSET(stdin_body.contents + cindex, newline));

            cindex = CHAR_OFFSET(stdin_body.contents, newline) + 1;
            
            continue;
        }

        fprintf(LIBERROR_STREAM, PROGRAM_NAME ": line %i is empty\n",
                common_errors_get_line(stdin_body, cindex) + 1);
        exit(EXIT_LINE_DOESNT_START_WITH_AT);
    }
}

void error_unclosed_tag(struct CString stdin_body, const char *tag) {
    int cindex = 0;
    int in_tag = 0;

    LIBERROR_IS_NULL(tag);
    VERIFY_CSTRING(stdin_body);

    while(cindex < stdin_body.length) {
        char *newline = NULL;

        LIBERROR_OUT_OF_BOUNDS(cindex, stdin_body.length);
        newline = strchr(stdin_body.contents + cindex, '\n');

        LIBERROR_IS_NULL(newline);
        LIBERROR_IS_NOT_VALUE(*newline, '\n');
        LIBERROR_IS_NEGATIVE(CHAR_OFFSET(stdin_body.contents + cindex, newline));

        /* We can make the assumption that each line starts with an '@' based off
         * of the previous checks */
        if(strncmp(stdin_body.contents + cindex, tag, strlen(tag)) == 0)
            INVERT_BOOLEAN(in_tag); 

        cindex = CHAR_OFFSET(stdin_body.contents, newline) + 1;
    }

    if(in_tag == 0)
        return;

    fprintf(LIBERROR_STREAM, PROGRAM_NAME ": unclosed '%s' tag\n", tag);
    exit(EXIT_UNCLOSED_TAG);
}

void error_tag_too_long(struct CString stdin_body) {
    int cindex = 0;
    int in_multiline_tag = 0;

    VERIFY_CSTRING(stdin_body);

    /* Verify that each line's tag name is not too long.
     * The delimiter of the tag is the first non-alphabetical
     * character. */
    while(cindex < stdin_body.length) {
        char *newline = NULL;
        const char *separator = NULL;

        /* Just to clarify, we do the + 1 because of the start of
         * each line being an '@', which is, of course, not an alphabetical
         * character. */
        LIBERROR_OUT_OF_BOUNDS(cindex, stdin_body.length);
        newline = strchr(stdin_body.contents + cindex, '\n');
        separator = strpbrk(stdin_body.contents + cindex + 1, CLASS_NON_ALPHA);

        LIBERROR_IS_NULL(newline);
        LIBERROR_IS_NULL(separator);
        LIBERROR_IS_NOT_VALUE(*newline, '\n');
        LIBERROR_IS_NEGATIVE(CHAR_OFFSET(stdin_body.contents + cindex, newline));

        /* This tag is too long. */
        if(CHAR_OFFSET(stdin_body.contents + cindex, separator) > TAG_LENGTH) {
            fprintf(LIBERROR_STREAM, PROGRAM_NAME ": tag on line %i is too long\n", common_errors_get_line(stdin_body, cindex) + 1);
            exit(EXIT_TAG_TOO_LONG);
        }

        cindex = CHAR_OFFSET(stdin_body.contents, newline) + 1;
    }
}

void error_unrecognized_tag(struct CString stdin_body) {
    int cindex = 0;
    int in_multiline_tag = 0;

    VERIFY_CSTRING(stdin_body);

    while(cindex < stdin_body.length) {
        char *newline = NULL;

        LIBERROR_OUT_OF_BOUNDS(cindex, stdin_body.length);
        newline = strchr(stdin_body.contents + cindex, '\n');

        LIBERROR_IS_NULL(newline);
        LIBERROR_IS_NOT_VALUE(*newline, '\n');
        LIBERROR_IS_NEGATIVE(CHAR_OFFSET(stdin_body.contents + cindex, newline));

        /* Enter a multiline tag state so we do not perform checks for
         * unrecognized tags in the body of a multiline tag. We can be
         * sure that this will eventually end up leaving the tag because
         * because of the previous checks for unclosed tags. */
        TOGGLE_MULTILINE_TAG("@description\n");
        TOGGLE_MULTILINE_TAG("@notes\n");
        TOGGLE_MULTILINE_TAG("@examples\n");
        TOGGLE_MULTILINE_TAG("@arguments\n");
    }
}

int main(void) {
    struct CString stdin_body = cstring_loads(stdin);

    error_ends_without_newline(stdin_body);
    error_empty_line(stdin_body);
    error_line_doesnt_start_with_at(stdin_body);
    error_unclosed_tag(stdin_body, "@description\n");
    error_unclosed_tag(stdin_body, "@notes\n");
    error_unclosed_tag(stdin_body, "@examples\n");
    error_tag_too_long(stdin_body);

    cstring_free(stdin_body);

    return EXIT_SUCCESS;
}
