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
*/

#include "../docgen.h"

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

void writer_validate_string(const char *string) {
    int cindex = 0;
    int in_list = 0;
    int in_table = 0;
    int length = strlen(string);
    struct LibmatchCursor string_cursor = libmatch_cursor_init((char *) string, strlen(string));

    /* Detect incomplete markers and unrecognized ones */
    for(cindex = 0; cindex < length; cindex++) {
        char first_character = string[cindex];
        char second_character = -1;

        if((cindex + 1) <= length)
            second_character = string[cindex + 1];

        if(first_character != '\\')
            continue;

        /* Second character being -1 when the first character
         * is a backslash means that this marker is incomplete.
         * It can be grouped in with the switch for detecting
         * unrecognized options. */
        if(second_character == -1) {
            fprintf(stderr, "docgen: character at index %i is an incomplete marker\n", cindex);
            exit(EXIT_FAILURE); 
        }

        /* Legal characters for a marker */
        switch(second_character) {
            case 'E': case 'S': case 'B':
            case 'I': case 'L': case 'T':
            case 'H': case '\\':
                cindex++;

                continue;
        }

        fprintf(stderr, "docgen: character '%c' at index %i is not a recognzied marker\n", string[cindex + 1], cindex +  1);
        exit(EXIT_FAILURE); 
    }

    /* Verify that there are no tables in lists or lists in tables */
    for(cindex = 0; cindex < length; cindex++) {
        char first_character = string[cindex];
        char second_character = -1;

        if(first_character != '\\')
            continue;

        /* Previous checks garuntee that each \ is not incomplete, and so we
         * can index past cindex without worrying about indexing out of bounds. */
        second_character = string[cindex + 1];

        /* Irrelevant sequences */
        if(second_character != 'T' && second_character != 'L') {
            cindex++;

            continue;
        }

        /* Make sure there are no lists inside of this. */
        if(second_character == 'T') {
            if(in_list == 1) {
                fprintf(stderr, "Character at index %i begins a table inside of a list\n", cindex + 1); 
                exit(EXIT_FAILURE);
            }

            INVERT_BOOLEAN(in_table);
        }

        if(second_character == 'L') {
            if(in_table == 1) {
                fprintf(stderr, "Character at index %i begins a list inside of a table\n", cindex + 1); 
                exit(EXIT_FAILURE);
            }

            INVERT_BOOLEAN(in_list);
        }

        cindex++;
    }

    /* Reset these so that we can reliably tell which are not closed
     * based off which are set to 1 when the loop ends. */
    in_list = 0;
    in_table = 0;

    /* Verify that there are no unclosed tables and lists */
    for(cindex = 0; cindex < length; cindex++) {
        char first_character = string[cindex];
        char second_character = -1;

        if(first_character != '\\')
            continue;

        /* Previous checks garuntee that each \ is not incomplete, and so we
         * can index past cindex without worrying about indexing out of bounds. */
        second_character = string[cindex + 1];

        /* We do not have to worry about the second character being a list either
         * because of previous checks. */
        if(second_character == 'T')
            INVERT_BOOLEAN(in_table);

        if(second_character == 'L')
            INVERT_BOOLEAN(in_list);

        cindex++;
    }

    if(in_list == 1) {
        fprintf(stderr, "%s", "docgen: unclosed list inside of output\n");    
        exit(EXIT_FAILURE);
    }

    if(in_table == 1) {
        fprintf(stderr, "%s", "docgen: unclosed table inside of output\n");    
        exit(EXIT_FAILURE);
    }

    /* Verify that tables are correctly formatted. First find a table,
     * then parse it, then move onto the next */
    while(string_cursor.cursor < string_cursor.length) {
        int original_location = -1;
        int first_character = libmatch_cursor_getch(&string_cursor);
        int second_character = -1;

        /* Ignore non marker sequences */
        if(first_character != '\\')
            continue;

        second_character = libmatch_cursor_getch(&string_cursor);

        /* We only care about tables here. */
        if(second_character != 'T')
            continue;

        ASSERT_GETCH(&string_cursor, '\n');

        /* Next line should be a \S to specify the separator, and
         * should be follow this regular expression:
         *
         * \\S .\n
        */
        ASSERT_GETCH(&string_cursor, '\\');
        ASSERT_GETCH(&string_cursor, 'S');
        ASSERT_GETCH(&string_cursor, ' ');
        ASSERT_GETCH_CLASS(&string_cursor, LIBMATCH_PRINTABLE);
        ASSERT_GETCH(&string_cursor, '\n');

        /* Next line should be the header. */
        ASSERT_GETCH(&string_cursor, '\\');
        ASSERT_GETCH(&string_cursor, 'H');
        ASSERT_GETCH(&string_cursor, ' ');

        /* Make sure the header is not empty, and go to the end of the line if it is.*/
        ASSERT_GETCH_CLASS(&string_cursor, LIBMATCH_PRINTABLE);
        libmatch_next_line(&string_cursor);

        original_location = string_cursor.cursor;

        /* While the last does not start with a `\T\n`, verify that
         * it is an element `\E`, and is not empty. */
        while(libmatch_string_expect(&string_cursor, "\\T\n") == 0) {
            string_cursor.cursor = original_location;

            ASSERT_GETCH(&string_cursor, '\\');
            ASSERT_GETCH(&string_cursor, 'E');
            ASSERT_GETCH(&string_cursor, ' ');
            ASSERT_GETCH_CLASS(&string_cursor, LIBMATCH_PRINTABLE);
            libmatch_next_line(&string_cursor);

            original_location = string_cursor.cursor;
        }

        printf("!\n");
    }
}

void dump_cstring(const char *format, struct CString string, char output_path[LIBPATH_MAX_PATH + 1]) {
    int cindex = 0;
    int parsing_marker = 0;
    FILE *output_file = NULL;
    struct WriterParams settings;

    liberror_is_null(dum_cstring, format);
    INIT_VARIABLE(settings);

    /* Determine the settings to use for interpretingg markers */
    settings = select_format_settings(format);
    output_file = fopen(output_path, "w");

    if(output_file == NULL)
        liberror_failure(dump_cstring, fopen); 

    /* Write each character, and interpret marker sequences */
    for(cindex = 0; cindex < string.length; cindex++) {
        char first_character = string.contents[cindex];
        char second_character = -1;

        /* Also keep track of the next character, but it might be out
         * of bounds, so make suure we do not index out of boundss. */
        if((cindex + 1) < string.length)
            second_character = string.contents[cindex + 1];

        /* Is this a match sequence? */
        if(first_character == '\\') {
            switch(second_character) {
                case '\\':
                    fputc('\\', output_file);
                    break;

                /* Start or end bold. */
                case 'B': SWAP_MARKER_STATE(parsing_marker, bold); break;

                /* Start or end italics. */
                case 'I': SWAP_MARKER_STATE(parsing_marker, italics); break;

                default:
                    fprintf(stderr, "docgen: unrecognized marker '\\%c'\n", second_character);
                    exit(EXIT_FAILURE);

                    break;
            }

            /* Consider two characters parsed, so add an extra offset */
            cindex++;

            continue;
        }

        fputc(first_character, output_file);
    }

    fclose(output_file);
}
