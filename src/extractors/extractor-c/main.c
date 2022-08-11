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
 * This file is the entry point of the program that filters all non-docgen code
 * out of the stdin, and will dump it back out. Before it does all of this, it
 * will perform basic error checks on the input like verifying that each docgen
 * tag has a matching tag.
*/

#include <time.h>
#include <string.h>
#include <stdlib.h>

#include "../../docgen.h"
#include "../../common/errors/errors.h"

#include "main.h"

#define HANDLE_COMMENTS_AND_STRINGS()                                       \
    /* Skip to the next double quote. The function called here will         \
     * produce an error yelling at the user if the end of the file is       \
     * encountered and no ending quote is found. Also, we can still         \
     * find double quotes in comments, so we do not do this check when      \
     * we are in comments. */                                               \
    if(character == '"' && in_comment == 0) {                               \
        index++;                                                            \
        index += skip_double_quote(stdin_body, index);                      \
                                                                            \
        continue;                                                           \
    }                                                                       \
                                                                            \
    if(character == '\'' && in_comment == 0) {                              \
        index++;                                                            \
        index += skip_single_quote(stdin_body, index);                      \
                                                                            \
        continue;                                                           \
    }                                                                       \
                                                                            \
    /* We are in a comment now. We can assume we are not in a string        \
     * because of the other checks that would skip strings. */              \
    if(strncmp(stdin_body.contents + index, "/*", 2) == 0) {                \
        in_comment = 1;                                                     \
        index += strlen("/*");                                              \
                                                                            \
        continue;                                                           \
    }                                                                       \
                                                                            \
    /* We are no longer in a comment. Same logic applies as the above. */   \
    if(strncmp(stdin_body.contents + index, "*/", 2) == 0) {                \
        in_comment = 0;                                                     \
        index += strlen("/*");                                              \
                                                                            \
        continue;                                                           \
    }

int skip_double_quote(struct CString body, int offset) {
    int escaped = 0;
    int caller_offset = 0;

    VERIFY_CSTRING(body);
    LIBERROR_OUT_OF_BOUNDS(offset, body.length);

    while(offset < body.length) {
        int character = 0;

        LIBERROR_OUT_OF_BOUNDS(offset, body.length);
        character = body.contents[offset];

        /* Skip two characters, and toggle the escaped so that
         * if (offset += 2 < body.length), then we can
         * differentiate between an incomplete escape, and an
         * unclosed string */
        if(character == '\\') {
            escaped = 1;
            offset += 2;
            caller_offset += 2;

            continue; 
        }

        /* This condition will never be met if a backslash is met
         * immediately before this character, since a backslash
         * causes the offset to be incremented twice, past its
         * character. Also, note that the (+1) is so that the cursor
         * is immediately positioned AFTER the string. */
        if(character == '"')
            return caller_offset + 1;

        offset++;
        caller_offset++;
        escaped = 0;
    }

    /* If this point is ever reached, we know that the string
     * is never closed, or an escape is incomplete. */
    fprintf(LIBERROR_STREAM, "%s", PROGRAM_NAME ": input file has unclosed string\n");
    exit(EXIT_UNCLOSED_STRING);
}

int skip_single_quote(struct CString body, int offset) {
    int escaped = 0;
    int caller_offset = 0;

    VERIFY_CSTRING(body);
    LIBERROR_OUT_OF_BOUNDS(offset, body.length);

    while(offset < body.length) {
        int character = 0;

        LIBERROR_OUT_OF_BOUNDS(offset, body.length);
        character = body.contents[offset];

        /* Skip two characters, and toggle the escaped so that
         * if (offset += 2 < body.length), then we can
         * differentiate between an incomplete escape, and an
         * unclosed string */
        if(character == '\\') {
            escaped = 1;
            offset += 2;
            caller_offset += 2;

            continue; 
        }

        /* This condition will never be met if a backslash is met
         * immediately before this character, since a backslash
         * causes the offset to be incremented twice, past its
         * character. Also, note that the (+1) is so that the cursor
         * is immediately positioned AFTER the string. */
        if(character == '\'')
            return caller_offset + 1;

        offset++;
        caller_offset++;
        escaped = 0;
    }

    /* If this point is ever reached, we know that the string
     * is never closed, or an escape is incomplete. */
    fprintf(LIBERROR_STREAM, "%s", PROGRAM_NAME ": input file has unclosed string\n");
    exit(EXIT_UNCLOSED_STRING);
}

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

/*
 * Produce an error message if the stdin body contains an unmatched
 * docgen tag.
*/
void error_unmatched_docgen(struct CString stdin_body) {
    int index = 0;
    int in_comment = 0;
    int last_tag_line = 0;
    int in_docgen_tag = 0;

    VERIFY_CSTRING(stdin_body);

    while(index < stdin_body.length) {
        int character = 0;

        LIBERROR_OUT_OF_BOUNDS(index, stdin_body.length);
        character = stdin_body.contents[index];

        HANDLE_COMMENTS_AND_STRINGS()

        /* If this is not the docgen tag, then ignore it. */
        if(strncmp(stdin_body.contents + index, "@docgen\n", strlen("@docgen\n")) != 0) {
            index++;

            continue;
        }

        /* Record where we found the opening docgen tag */
        if(in_docgen_tag == 0)
            last_tag_line = common_errors_get_line(stdin_body, index);

        INVERT_BOOLEAN(in_docgen_tag);
        index += strlen("@docgen\n");
    }

    /* Once the loop ends, if we are still "in a comment" then we can produce
     * an error yelling at the luser. */
    if(in_comment == 1) {
        fprintf(LIBERROR_STREAM, "%s", PROGRAM_NAME ": input file has unclosed comment\n");
        exit(EXIT_UNCLOSED_COMMENT);

        return;
    }

    /* However, if the loop ends and we have not met a matching "docgen" tag,
     * we got a problem. */
    if(in_docgen_tag == 0)
        return; 

    fprintf(LIBERROR_STREAM, PROGRAM_NAME ": input file has unclosed docgen tag on line %i\n",
           last_tag_line + 1);
    exit(EXIT_UNCLOSED_DOCGEN);
}

/*
 * Verifies that the end of a comment does not appear on the same line
 * as a docgen tag.
*/
void error_comment_end_on_tag_line(struct CString stdin_body) {
    int index = 0;
    int in_comment = 0;
    int last_tag_line = 0;
    int in_docgen_tag = 0;

    VERIFY_CSTRING(stdin_body);

    while(index < stdin_body.length) {
        int character = 0;
        char *newline_location = NULL;

        LIBERROR_OUT_OF_BOUNDS(index, stdin_body.length);
        character = stdin_body.contents[index];

        HANDLE_COMMENTS_AND_STRINGS()

        /* Do not bother with characters that are not '@' signs */
        if(character != '@') {
            index++;

            continue; 
        }

        /* Toggle the docgen tag to let us know that we can now display docgen
         * tags */
        if(strncmp(stdin_body.contents + index, "@docgen\n", strlen("@docgen\n")) == 0) {
            INVERT_BOOLEAN(in_docgen_tag);
            index += strlen("@docgen\n");

            continue;
        }

        /* Only count '@' signs as actual docgen tags if we are IN a docgen block */
        if(in_docgen_tag == 0) {
            index++;

            continue; 
        }

        /* We know there will always be a new line, so we can replace the
         * new line temporarily so we can check the line, and then put it back. */
        newline_location = strchr(stdin_body.contents + index, '\n');
        LIBERROR_IS_NULL(newline_location);
        LIBERROR_IS_NOT_VALUE(*newline_location, '\n');

        /* There cannot be an end of multiline comment delimiter on the same line
         * as this docgen tag */
        if(strstr(stdin_body.contents + index, "*/") != NULL) {
            fprintf(LIBERROR_STREAM, PROGRAM_NAME ": '*/' detected on the same line as a docgen tag on line %i\n",
                    common_errors_get_line(stdin_body, index) + 1);
            exit(EXIT_CLOSED_ON_DOCGEN_TAG);
        }

        index = CHAR_OFFSET(stdin_body.contents, newline_location);
    }
}

/*
 * Scan the stdin (through the consumed cstring) for any docgen
 * tags. A docgen tag is defined as anything that is between
 * two opening tags, 'docgen.' For each tag it finds, it will
 * write it to the stdout.
*/
void scan_stdin(struct CString stdin_body) {
    int index = 0;
    int in_comment = 0;
    int in_docgen_tag = 0;

    VERIFY_CSTRING(stdin_body);

    while(index < stdin_body.length) {
        int character = 0;
        char *newline_location = NULL;

        LIBERROR_OUT_OF_BOUNDS(index, stdin_body.length);
        character = stdin_body.contents[index];

        HANDLE_COMMENTS_AND_STRINGS()

        /* Do not bother with characters that are not '@' signs */
        if(character != '@') {
            index++;

            continue; 
        }

        /* Toggle the docgen tag to let us know that we can now display docgen
         * tags */
        if(strncmp(stdin_body.contents + index, "@docgen\n", strlen("@docgen\n")) == 0) {
            INVERT_BOOLEAN(in_docgen_tag);
            index += strlen("@docgen\n");

            printf("@docgen\n");

            continue;
        }

        /* Only count '@' signs as actual docgen tags if we are IN a docgen block */
        if(in_docgen_tag == 0) {
            index++;

            continue; 
        }

        /* We know there will always be a new line, so we can replace the
         * new line temporarily so printf will not display past it, and then
         * put it back. */
        newline_location = strchr(stdin_body.contents + index, '\n');

        LIBERROR_IS_NULL(newline_location);
        LIBERROR_IS_NOT_VALUE(*newline_location, '\n');

        newline_location[0] = '\0';
        printf("%s\n", stdin_body.contents + index);
        index = CHAR_OFFSET(stdin_body.contents, newline_location);
        newline_location[0] = '\n';
    }
}

int main(void) {
    struct CString stdin_body = cstring_loads(stdin);

    error_ends_without_newline(stdin_body);    
    error_comment_end_on_tag_line(stdin_body);
    error_unmatched_docgen(stdin_body);
    scan_stdin(stdin_body);

    cstring_free(stdin_body);

    return EXIT_SUCCESS;
}
