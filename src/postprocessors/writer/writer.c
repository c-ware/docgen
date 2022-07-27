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
 * This file contains functions used for writing the final string into the file
 * of choice. Writers is part of the postprocessor step, and while the postprocessor
 * function handles the arrangement of the output, the writer writes it and will
 * handle markers which provide additional formatting like bolding, italcs, and tables.
 *
 * This quite possibly had to be the single most annoying thing to write. Not the thing
 * thath andles the bold and italics, but rather just the tables and lists. Most of my
 * frustration comes from my own doing, which is that in the manual pages, I have .br's
 * put in between each line. This is fine for the most part, but the problem is that
 * it adds them in between the table and list markers.
 *
 * This on its own is not that big of a problem, but I do want to have fairly strict
 * rules on the formatting of tables and lists. Specifically, tables and lists should
 * not have any arbitrary text / whitespace in between each element / segment. Adding
 * .br's throws a wrench in that.
 * 
 * What I have decided to do to combat this is to have the function which adds breaks
 * in between each line execute an error checking function which will scan the input
 * string its given to write and detect any unclosed tables or lists. When this is
 * done, the break adder can reliably REFUSE to add .br's inside of the lines of a
 * table or list, and then when it comes time to translate the table syntax, I can
 * parse and produce errors for it.
 *
 * Errors are also fairly difficult. The strings the postprocessing functions have
 * access to are several layers above the actual cursors used to extract tokens
 * from the source and header files, and by the time its in the postprocessor, it
 * is impractical to determine which line belongs to which part because of the
 * potential of empty lines being in the comment, which are ignored by the
 * extractors, so at the very least we have to settle for errors like
 * "docgen: expected x but got y\n" for now, unfortunately. Despite this, we can
 * still have actual errors produced by the extractor code. Maybe in the future
 * we could have the extractor code produce errors for invalid table layouts?
 * Maybe we could also add a bit of context to the errors as well, like
 * "expected x in description but got y\n". This would be pretty OK all things
 * considered, but we would not have line numbers.
*/

#include "../../docgen.h"
#include "../postprocessors.h"

#include "writer.h"

/*
 * @docgen: function
 * @brief: determine the formatting settings forr markers
 * @name: select_format_settings
 *
 * @description
 * @This function will fill a structure with settings like bold and italc
 * @characters which are used by the writer.
 * @description
 * 
 * @error: format is NULL
 * @error: unknown format
 *
 * @param format: the format the writer is using
 * @type: const char *
 *
 * @return: the writer settings
 * @type: struct WriterParams
*/
static struct WriterParams select_format_settings(const char *format) {
    struct WriterParams settings;

    INIT_VARIABLE(settings);
    liberror_is_null(select_format_settings, format);

    if(strcmp(format, "manpage") ==  0) {
        settings.bold_start = "\\fB"; 
        settings.italics_start = "\\fI"; 
        settings.bold_end = "\\fR"; 
        settings.italics_end = "\\fR"; 
    } else {
        liberror_unhandled(select_format_settings);
    }

    return settings;
}

static char *get_next_marker(struct LibmatchCursor *cursor) {
    while(cursor->cursor < cursor->length) {
        char *next_line = libmatch_read_alloc_until(cursor, "\n");

        libmatch_cursor_getch(cursor);

        /* Immediately invalid line-- a marker is \x */
        if(strlen(next_line) < 2) {
            free(next_line); 

            continue;
        }

        /* Not a marker */
        if(next_line[0] != '\\') {
            free(next_line); 

            continue;
        }

        return next_line;
    }

    return NULL;
}

static void validate_separator(const char *line) {
    struct LibmatchCursor cursor = libmatch_cursor_init((char *) line, strlen(line));

    ASSERT_GETCH(&cursor, '\\');
    ASSERT_GETCH(&cursor, 'S');
    ASSERT_GETCH(&cursor, ' ');
    ASSERT_GETCH_CLASS(&cursor, LIBMATCH_PRINTABLE);
}

static void assert_not_empty(const char *line) {
    struct LibmatchCursor cursor = libmatch_cursor_init((char *) line, strlen(line));

    ASSERT_GETCH(&cursor, '\\');
    ASSERT_GETCH_CLASS(&cursor, LIBMATCH_ALPHA);
    ASSERT_GETCH(&cursor, ' ');
    ASSERT_GETCH_CLASS(&cursor, LIBMATCH_PRINTABLE);
}

static struct Table parse_table(struct LibmatchCursor *cursor) {
    struct Table new_table;
    char *next_marker = get_next_marker(cursor);

    INIT_VARIABLE(new_table);

    new_table.sections = carray_init(new_table.sections, CSTRING);

    /* Iterate through each marker */
    while(next_marker != NULL) {
        /* End of the table */
        if(strcmp(next_marker, "\\T") == 0) {
            free(next_marker);

            break;
        }

        /* Parse a separator */
        if(strncmp(next_marker, "\\S", 2) == 0) {
            validate_separator(next_marker);

            new_table.separator = next_marker[strlen(next_marker) - 1];
        }

        /* Parse a header */
        if(strncmp(next_marker, "\\H", 2) == 0) {
            struct CString header_string;

            INIT_VARIABLE(header_string);
            header_string.contents = malloc(strlen(next_marker) - strlen("\\H ") + 1);
            memset(header_string.contents, 0, strlen(next_marker) - strlen("\\H ") + 1);

            /* Verify that the header is not empty, and create a copy of the parsed header. */
            assert_not_empty(next_marker);
            strncat(header_string.contents, next_marker + strlen("\\H "), strlen(next_marker) - strlen("\\H "));
            header_string.length = strlen(header_string.contents);
            header_string.capacity = header_string.length + 1;
            new_table.header = header_string;
        }

        /* Parse an element */
        if(strncmp(next_marker, "\\E", 2) == 0) {
            struct CString section_string;

            INIT_VARIABLE(section_string);
            section_string.contents = malloc(strlen(next_marker) - strlen("\\E ") + 1);
            memset(section_string.contents, 0, strlen(next_marker) - strlen("\\E ") + 1);

            /* Verify that the section is not empty, and create a copy of the parsed section. */
            assert_not_empty(next_marker);
            strncat(section_string.contents, next_marker + strlen("\\E "), strlen(next_marker) - strlen("\\E "));
            section_string.length = strlen(section_string.contents);
            section_string.capacity = section_string.length + 1;

            carray_append(new_table.sections, section_string, CSTRING);
        }

        /* Free either the correct marker, or ignore this one. */
        free(next_marker);
        next_marker = get_next_marker(cursor);
    }

    return new_table;
}

void dump_cstring(const char *format, struct CString string, char output_path[LIBPATH_MAX_PATH + 1]) {
    int cindex = 0;
    int parsing_marker = 0;
    FILE *output_file = NULL;
    struct WriterParams settings;
    struct LibmatchCursor cursor = libmatch_cursor_init(string.contents, string.length);

    liberror_is_null(dum_cstring, format);
    INIT_VARIABLE(settings);

    /* Determine the settings to use for interpretingg markers */
    settings = select_format_settings(format);
    output_file = fopen(output_path, "w");

    /* Perform basic sanity checks on the markers */
    no_unrecognized_markers(string.contents);
    no_nested_elements(string.contents);
    no_unclosed_elements(string.contents);
    one_table_marker_per_line(string.contents);

    if(output_file == NULL)
        liberror_failure(dump_cstring, fopen); 

    /* Read and interpret individual lines. */
    while(cursor.cursor < cursor.length) {
        char *line = libmatch_read_alloc_until(&cursor, "\n");
        libmatch_cursor_getch(&cursor);

        /* Parse a table if the line starts with it (the line will not
         * have any new line at the end of it) */
        if(strcmp(line, "\\T") == 0) {
            int sindex = 0;
            struct Table parsed_table = parse_table(&cursor);

            for(sindex = 0; sindex < carray_length(parsed_table.sections); sindex++) {
                printf("Section: '%s'\n", parsed_table.sections->contents[sindex].contents); 
            }

            exit(EXIT_FAILURE);
        }

        /*
        printf("Line: '%s'\n", line);
        */
        free(line);
    }

    fclose(output_file);
}
