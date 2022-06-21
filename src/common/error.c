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
 * This file contains "error checkers" for tags. A more unconscious design
 * choice of docgen that I later came to dislike was how I originally mixed
 * the extraction of different parts of a tag with the error handling.
 *
 * In my opinion, it made it messy, and it gave the functions used for
 * extracting the parts of the tag have a dual-purpose, and made it harder to
 * differentiate extraction code from code that was used for error handling
 * later in the function, so I have decided to separate the two processes, with
 * the only caveat being that the extraction function will call its associated
 * error checker, THEN do parsing with the assumption that the tag is correct.
*/

#include <stdio.h>

#include "../docgen.h"
#include "common.h"

/*
 * This function will enforce this regular expression on the line passed
 * to it as an argument, producing errors to the stderr if it is violated:
 * @TAGNAME: {1}.+
 *
 * This being said, however, this does not enforce anything on the tag
 * name (including whether or not the : appears) because verifying a correct
 * tag is something that happens outside of this. When this function is called,
 * it should be assumed that the callee knows the tag is correct.
 *
 * However, in the future, it might not hurt to write a function for scanning
 * the tag at the start of a line for syntactic validity rather than semantic
 * validity.
*/
void field_line_error_check(const char *line, int line_number) {
    char *colon_separator = NULL;

    liberror_is_null(field_line_error_check, line);
    liberror_is_negative(field_line_error_check, line_number);

    colon_separator = strchr(line, ':');

    if(colon_separator == NULL) {
        fprintf(stderr, "docgen: tag on line %i has no colon (:)\n", line_number);
        exit(EXIT_FAILURE);
    }

    /* First things first. A tag name with a colon MUST be followed
     * by a SINGLE space. */
    if(*(colon_separator + 1) != ' ') {
        fprintf(stderr, "docgen: tag on line %i must be followed immediately by a space\n",
                line_number);
        exit(EXIT_FAILURE);
    }

    /* After the space, there must be at least one non-whitespace character--
     * this is to make sure the line is not left empty. */
    if(*(colon_separator + 2) == '\0' || strchr(LIBMATCH_WHITESPACE, *(colon_separator + 2)) != NULL) {
        fprintf(stderr, "docgen: tag on line %i must have data assigned\n", line_number);
        exit(EXIT_FAILURE);
    }
}

/*
 * This function will enforce this regular expression on the line passed
 * to it as an argument, producing errors to the stderr if it is violated:
 * @TAGNAME {1}[a-zA-Z].*: [^\s]+
*/
void field_line_arg_error_check(const char *line, int line_number) {
    int character = -1;
    struct LibmatchCursor cursor;

    liberror_is_null(field_line_arg_error_check, line);
    liberror_is_negative(field_line_arg_error_check, line_number);

    cursor = libmatch_cursor_init((char *) line, strlen(line));

    libmatch_until(&cursor, "@");

    /* First thing to enforce: first non-alphabetical character after the
     * name of the tag must be a space. */
    while(cursor.cursor < cursor.length) {
        int character = libmatch_cursor_getch(&cursor);

        /* This is an alphabetical character */
        if(strchr(LIBMATCH_ALPHA, character) != NULL)
            continue;

        if(character == ' ')
            break;

        fprintf(stderr, "docgen: name of the tag on line %i must be directly "
                "followed by a space-- got '%c'\n", line_number, character);
        exit(EXIT_FAILURE);
    }

    /* After the space that separates the tag from the argument, and the
     * field, there must be an alphabetical character. */
    character = libmatch_cursor_getch(&cursor);

    if(character == LIBMATCH_EOF || strchr(LIBMATCH_ALPHA, character) == NULL) {
        fprintf(stderr, "docgen: space after tag name on line %i must have an alphabetical "
                "character immediately after, got '%c'\n", line_number, character);
        exit(EXIT_FAILURE);
    }

    /* Does this tag even have a colon? */
    if(libmatch_cond_before(&cursor, ':', "\n") == 0) {
        fprintf(stderr, "docgen: tag on line %i missing colon (:)\n", line_number);
        exit(EXIT_FAILURE);
    }

    /* Is every character until the colon not whitespace? */
    while(cursor.cursor < cursor.length) {
        int character = libmatch_cursor_getch(&cursor);

        if(character == ':')
            break;
        
        if(strchr(LIBMATCH_PRINTABLE, character) != NULL)
            continue;

        fprintf(stderr, "docgen: argument to tag on line %i has whitespace before colon\n",
                line_number);
        exit(EXIT_FAILURE);
    }

    /* Space should immediately follow a colon (:) */
    character = libmatch_cursor_getch(&cursor);

    if(character != ' ') {
        fprintf(stderr, "docgen: colon after tag name on line %i must have a space ( ) "
                "character immediately after, got '%c'\n", line_number, character);
        exit(EXIT_FAILURE);
    }

    /* After the space there must be a non-whitespace character */
    character = libmatch_cursor_getch(&cursor);

    if(character == LIBMATCH_EOF || strchr(LIBMATCH_PRINTABLE, character) == NULL) {
        fprintf(stderr, "docgen: space after colon on line %i must be directly followed by "
                "a printable character\n", line_number);
        exit(EXIT_FAILURE);
    }
}

/*
 * This function will enforce this regular expression on each line
 * inside of a block. This function itself does NOT traverse each
 * line itself, but rather relies on the caller to extract the
 * next line.
 *
 * The regular expression that is enforced is:
 * @.+
 *
 * The only real thing that needs to be 'enforced' here is the
 * presence of a @ at the start of the line, but who knows
 * what the future might hold.
*/
void block_error_check(const char *line, int line_number) {
    struct LibmatchCursor cursor;

    liberror_is_null(block_error_check, line);
    liberror_is_negative(block_error_check, line_number);

    cursor = libmatch_cursor_init((char *) line, strlen(line));

    if(libmatch_cond_before(&cursor, '\n', "@") == 0)
        return;

    fprintf(stderr, "docgen: unterminated block on line %i\n", line_number);
    exit(EXIT_FAILURE);
}
