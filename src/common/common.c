/*
 * C-Ware License
 * 
 * Copyright (c) 2022, C-Ware
 * All rights reserved.
 * * Redistribution and use in source and binary forms, with or without
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
 * This contains common functions that can be used in each token
 * parser.
*/

#include <unistd.h>
#include "../docgen.h"

#include "common.h"
#include "../extractors/structures/structures.h"

void docgen_extract_type(struct LibmatchCursor *cursor, char *buffer, int length) {
    liberror_is_null(docgen_extract_type, cursor);
    liberror_is_null(docgen_extract_type, buffer);
    liberror_is_negative(docgen_extract_type, length);

    /* Type should follow
     * TODO: Add checks for there being no type tag following this,
     * or the end of the comment. */
    libmatch_until(cursor, "@");

    /* TODO: Report this as an error */
    if(libmatch_string_expect(cursor, "type: ") == 0) {

    }

    /* Read the type */
    libmatch_read_until(cursor, buffer, length, "\n");
}

void docgen_parse_comment(struct LibmatchCursor *cursor) {
    liberror_is_null(docgen_parse_comment, cursor);

    libmatch_cursor_enable_pushback(cursor);

    while(cursor->cursor != cursor->length) {
        if(libmatch_string_expect(cursor, "*/") == 0) {
            continue;
        }

        break;
    }

    libmatch_cursor_disable_pushback(cursor);
}

int docgen_comment_is_type(struct LibmatchCursor *cursor, const char *comment_start,
                           const char *commend_end, const char *type) {
    /* From: single-comment mode support attempt
        struct DocgenTag new_tag;
        char name[DOCGEN_LINE_LENGTH + 1];

        memset(&new_tag, 0, sizeof(struct DocgenTag));

        new_tag = docgen_tag_next(cursor, comment_start, commend_end);

        while(new_tag.status != DOCGEN_TAG_STATUS_SUCCESS) {
            printf("Before Line: %s, %s, %s\n", name, type, new_tag.line);

            switch(new_tag.status) {
                case DOCGEN_TAG_STATUS_DONE:
                    return 0;

                case DOCGEN_TAG_STATUS_EOF:
                    return 0;

                case DOCGEN_TAG_STATUS_EOC:
                    return 0;

                case DOCGEN_TAG_STATUS_FULL:
                    return 0;
            }

            new_tag = docgen_tag_next(cursor, comment_start, commend_end);
            printf("After Line: %s, %s, %s\n", name, type, new_tag.line);
        }
        docgen_extract_field_line("docgen", DOCGEN_LINE_LENGTH, cursor->line, new_tag.line, name);

        struct DocgenTag new_tag;
        char name[DOCGEN_LINE_LENGTH + 1];

        memset(&new_tag, 0, sizeof(struct DocgenTag));

        new_tag = docgen_tag_next(cursor, comment_start, commend_end);

        return 1;
    */

    libmatch_next_line(cursor);

    liberror_is_null(docgen_comment_is_type, cursor);
    liberror_is_null(docgen_comment_is_type, type);

    /* After a comment is found, the next line must have a string
     * with a tag "@docgen" to signal that this is a doucmentation
     * generating comment */
    if(libmatch_cond_before(cursor, '@', "\n") == 0)
        return 0;

    libmatch_until(cursor, "@");

    /* Tag must be docgen */
    if(libmatch_string_expect(cursor, "docgen: ") == 0)
        return 0;

    /* Does the tag have an argument?
     * TODO: MAKE THIS FUNCTION PRODUCE AN ERROR!! */
    if(libmatch_cond_before(cursor, '\n', LIBMATCH_ALPHA) == 1)
        return 0;

    libmatch_cursor_enable_pushback(cursor);
    libmatch_until(cursor, LIBMATCH_ALPHA);
    libmatch_cursor_disable_pushback(cursor);

    /* Type must be 'function' */
    if(libmatch_string_expect(cursor, type) == 0)
        return 0;

    /* Jump to the next line */
    libmatch_next_line(cursor);

    return 1;
}

void docgen_extract_field_block(const char *tag_name, char *buffer, int length,
                                struct LibmatchCursor *cursor, char *tag_line) {
    int written = 0;
    int block_length = 0;
    struct LibmatchCursor buffer_cursor;
    char block_line[DOCGEN_BLOCK_LINE_LENGTH + 1];

    liberror_is_null(docgen_extract_field_block, tag_name);
    liberror_is_null(docgen_extract_field_block, buffer);
    liberror_is_null(docgen_extract_field_block, cursor);
    liberror_is_null(docgen_extract_field_block, read);
    liberror_is_negative(docgen_extract_field_block, length);

    buffer_cursor = libmatch_cursor_init(tag_line, strlen(tag_line));
    libmatch_until(&buffer_cursor, "@");

    /* The first non-alphabetical character must be a new line */
    while(buffer_cursor.cursor < buffer_cursor.length) {
        int character = libmatch_cursor_getch(&buffer_cursor);

        if(strchr(LIBMATCH_ALPHANUM "_", character) != NULL)
            continue;

        /* Character is non-alphabetic; first instance of this must be
         * a colon. Otherwise, error. */
        if(character == '\n')
            break;

        fprintf(stderr, "docgen: tag '%s' on line %i not immediately followed by a new line (\\n)\n",
                tag_name, cursor->line);
        exit(EXIT_FAILURE);
    }

    buffer[0] = '\0';

    /* Read the lines of the block until the end of the block */
    while(1) {
        struct LibmatchCursor line_cursor;
        char line_buffer[DOCGEN_BLOCK_LINE_LENGTH + 1];

        written = libmatch_read_until(cursor, block_line, DOCGEN_BLOCK_LINE_LENGTH, "\n");

        if(written == DOCGEN_BLOCK_LINE_LENGTH) {
            fprintf(stderr, "docgen: line %i in body of tag '%s' is too long. max of %i\n",
                    cursor->line, tag_name, DOCGEN_BLOCK_LINE_LENGTH);
            exit(EXIT_FAILURE);
        }

        /* Line must has a field sign */
        if(strchr(block_line, '@') == NULL) {
            fprintf(stderr, "docgen: line %i in body of tag '%s' has no @.\n", cursor->line,
                    tag_name);
            exit(EXIT_FAILURE);
        }

        line_cursor = libmatch_cursor_init(block_line, written);
        
        /* Parse this line and add it to the description. + 1 for the new
         * line. */
        libmatch_until(&line_cursor, "@");
        block_length += libmatch_read_until(&line_cursor, line_buffer, DOCGEN_BLOCK_LINE_LENGTH, "\n") + 1;

        /* End of the block */
        if(strcmp(line_buffer, tag_name) == 0)
            break;

        /* Report a block that is too big */
        if(block_length >= length) {
            fprintf(stderr, "docgen: body of tag '%s' is too long starting at line %i-- max of %i\n",
                    tag_name, cursor->line, length - 1);
            exit(EXIT_FAILURE);
        }

        /* No need to use strncat-- garunteed to be safe because of
         * preceeding checks. */
        strcat(buffer, line_buffer);
        strcat(buffer, "\n");
    }
}

const char *docgen_get_comment_start(struct DocgenArguments arguments) {
    if(strcmp(arguments.language, "c") == 0) {
        return "/*";
    }    

    /* From: single comment mode attempt
    if(strcmp(arguments.language, "py") == 0) {
        return "#";
    }    
    */

    liberror_unhandled(docgen_get_comment_start);

    return NULL;
}

const char *docgen_get_comment_end(struct DocgenArguments arguments) {
    if(strcmp(arguments.language, "c") == 0) {
        return "*/";
    }    

    /* From: single comment mode attempt
    if(strcmp(arguments.language, "py") == 0) {
        return "";
    }    
    */

    liberror_unhandled(docgen_get_comment_end);

    return NULL;
}

void docgen_create_file_path(struct DocgenArguments arguments, const char *name,
                             char *buffer, int length) {

    liberror_is_null(docgen_create_file_path, name);
    liberror_is_null(docgen_create_file_path, buffer);
    liberror_is_negative(docgen_create_file_path, length);

    libpath_join_path(buffer, length, "./doc/", name, ".", arguments.section, NULL);
}

void docgen_extract_field_line(const char *tag_name, char *buffer, int buffer_length,
                               int line_number, char *tag_line) {
    int written = 0;
    struct LibmatchCursor buffer_cursor;

    liberror_is_null(docgen_extract_field_line, tag_name);
    liberror_is_null(docgen_extract_field_line, buffer);
    liberror_is_null(docgen_extract_field_line, tag_line);
    liberror_is_negative(docgen_extract_field_line, buffer_length);
    liberror_is_negative(docgen_extract_field_line, line_number);

    /* Perform error checks on the line */
    field_line_error_check(tag_line, line_number);

    buffer_cursor = libmatch_cursor_init(tag_line, strlen(tag_line));

    /* A field line tag must have a : directly after its name.
     * Jump to the @, then the start of the :, and then
     * traverse past it to start reading the argument until the \n */
    libmatch_until(&buffer_cursor, "@");
    libmatch_until(&buffer_cursor, ":");
    libmatch_cursor_getch(&buffer_cursor);

    /* Read the name, and do some final error checks. */
    written = libmatch_read_until(&buffer_cursor, buffer, buffer_length, "\n");

    if(written >= buffer_length) {
        fprintf(stderr, "docgen: description of tag '%s' on line %i is too long-- max length of %i\n",
                tag_name, line_number, buffer_length);
        exit(EXIT_FAILURE);
    }
}


void docgen_extract_field_line_arg(const char *tag_name, char *argument_buffer,
                                   int argument_length, char *description_buffer,
                                   int description_length, int line_number, char *tag_line) {
    int written = 0;
    int buffer_cursor = 0;
    struct LibmatchCursor cursor;

    liberror_is_null(docgen_extract_field_line_arg, tag_name);
    liberror_is_null(docgen_extract_field_line_arg, tag_line);
    liberror_is_null(docgen_extract_field_line_arg, argument_buffer);
    liberror_is_null(docgen_extract_field_line_arg, description_buffer);
    liberror_is_negative(docgen_extract_field_line_arg, argument_length);
    liberror_is_negative(docgen_extract_field_line_arg, description_length);
    liberror_is_negative(docgen_extract_field_line_arg, line_number);

    /* Validate the line */
    field_line_arg_error_check(tag_line, line_number);
    cursor = libmatch_cursor_init(tag_line, strlen(tag_line));

    /* Jump to the @, then the first space to begin parsing the argument */
    libmatch_until(&cursor, "@");
    libmatch_until(&cursor, " ");

    /* Write the embed type, and do error checks along the way. */
    while(cursor.cursor < cursor.length) {
        int character = -1;

        character = libmatch_cursor_getch(&cursor);

        /* Stop at the : */
        if(character == ':')
            break;

        /* Can the buffer hold an extra character? */
        if(buffer_cursor == argument_length) {
            fprintf(stderr, "docgen: argument of tag '%s' on line %i is too long-- max of %i\n",
                    tag_name, line_number, argument_length);
            exit(EXIT_FAILURE);
        }

        argument_buffer[buffer_cursor] = character;
        buffer_cursor++;
    }

    libmatch_cursor_getch(&cursor);

    /* Read the description */
    written = libmatch_read_until(&cursor, description_buffer, description_length, "\n");

    if(written == description_length) {
        fprintf(stderr, "docgen: description to tag '%s' on line %i is too long-- max length of %i\n",
                tag_name, line_number, description_length);
        exit(EXIT_FAILURE);
    }

    argument_buffer[buffer_cursor] = '\0';
}

struct Reference docgen_extract_reference(struct LibmatchCursor *cursor,
                                          struct DocgenTag new_tag) {
    int written = 0;
    struct Reference new_reference;
    struct LibmatchCursor reference_cursor;
    char reference_line[DOCGEN_TAG_LINE_LENGTH + 1];

    liberror_is_null(docgen_extract_reference, cursor);

    docgen_extract_field_line("reference", reference_line, DOCGEN_TAG_LINE_LENGTH,
                              cursor->line, new_tag.line);

    if(strchr(reference_line, '(') == NULL) {
        fprintf(stderr, "docgen: tag 'reference' on line %i is missing opening parenthesis\n",
                 cursor->line);
        exit(EXIT_FAILURE);
    }

    if(strchr(reference_line, ')') == NULL) {
        fprintf(stderr, "docgen: tag 'reference' on line %i is missing closing parenthesis\n",
                 cursor->line);
        exit(EXIT_FAILURE);
    }

    reference_cursor = libmatch_cursor_init(reference_line, strlen(reference_line));

    /* Read the manual */
    written = libmatch_read_until(&reference_cursor, new_reference.manual,
                                  DOCGEN_MANUAL_NAME_LENGTH, "(");

    if(written == DOCGEN_MANUAL_NAME_LENGTH) {
        fprintf(stderr, "docgen: tag 'reference' manual name on line %i is too long-- max of %i\n",
                cursor->line, DOCGEN_MANUAL_NAME_LENGTH - 1);
        exit(EXIT_FAILURE);
    }

    /* Read the section */
    written = libmatch_read_until(&reference_cursor, new_reference.section,
                                  DOCGEN_MANUAL_SECTION_LENGTH, ")");

    if(written == DOCGEN_MANUAL_SECTION_LENGTH) {
        fprintf(stderr, "docgen: tag 'reference' manual name on line %i is too long-- max of %i\n",
                cursor->line, DOCGEN_MANUAL_SECTION_LENGTH - 1);
        exit(EXIT_FAILURE);
    }

    return new_reference;
}

void docgen_do_padding(struct DocgenStructureField field, int longest, int depth,
                       FILE *location) {
    int padding = 0;
    int padding_index = 0;

    liberror_is_null(docgen_extract_field_block, location);
    liberror_is_negative(docgen_extract_field_block, longest);
    liberror_is_negative(docgen_extract_field_block, depth);

    /* Add padding before the description */
    padding = (longest - docgen_get_field_length(field, depth));

    for(padding_index = 0; padding_index < padding; padding_index++) {
        fprintf(location, "%c", ' ');
    }
}
