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
    struct CString buffer_e;
*/

#ifndef DOCGEN_WRITER_H
#define DOCGEN_WRITER_H

#define INVERT_BOOLEAN(x) \
    ((x) = !(x))

/*
 * @docgen: macro_function
 * @brief: swap the marker state and dump the corresponding string
 * @name: SWAP_MARKER_STATE
 *
 * @description
 * @This macro function will swap a marker, and then based off of
 * @the state before the swap, print the setting that corresponds
 * @to that state. This is used in situations like swapping the
 * @state between bolding, italics, and tables. This is not used
 * @for every single situation, but it is used for some.
 * @description
*/
#define SWAP_MARKER_STATE(marker, setting)             \
    if((marker) == 0)                                  \
        fputs(settings. setting##_start, output_file); \
    else                                               \
        fputs(settings. setting##_end, output_file);   \
                                                       \
    INVERT_BOOLEAN(marker)

/*
 * @docgen: macro_function
 * @brief: assert that the next character is certain character
 * @name: ASSERT_GETCH
 *
 * @description
 * @Asserts that the next character is certain character, and dumps an error
 * @message if it is not there.
 * @description
 *
 * @param cursor: the cursor to getch
 * @param x: the character to expect
*/
#define ASSERT_GETCH(_cursor, x)                                                            \
do {                                                                                        \
    if((libmatch_cursor_getch((_cursor)) == (x)))                                           \
        break;                                                                              \
                                                                                            \
    fprintf(stderr, "docgen: expected '%c', got '%c' (line: %i)\n", (x),                    \
            (_cursor)->buffer[(_cursor)->cursor - 1], (_cursor)->line);                     \
    fprintf(stderr, "%s", "(Maintainers, remember that the writer is reading postprocessed" \
            " text. This is not reading from the file itself. This error might be caused"   \
            "by the documentation format postprocessor producing invalid syntax\n");        \
    exit(EXIT_FAILURE);                                                                     \
} while(0)

#define ASSERT_GETCH_CLASS(_cursor, _class)                                                 \
do {                                                                                        \
    if(strchr(_class, libmatch_cursor_getch((_cursor))) != NULL)                            \
        break;                                                                              \
                                                                                            \
    fprintf(stderr, "docgen: expected one of '%s', got '%c' (line: %i)\n", (_class),        \
            (_cursor)->buffer[(_cursor)->cursor - 1], (_cursor)->line);                     \
    fprintf(stderr, "%s", "(Maintainers, remember that the writer is reading postprocessed" \
            " text. This is not reading from the file itself. This error might be caused"   \
            "by the documentation format postprocessor producing invalid syntax\n");        \
    exit(EXIT_FAILURE);                                                                     \
} while(0)


/*
 * A more abstract representation of a table.
*/
struct Table {
    int length;
    int height;
    int separator;
    struct CString *sections;
};

/*
 * @docgen: function
 * @brief: verify that the string give to it contains no marker errors
 * @name: validate_output
 *
 * @description
 * @Given an input string, it will validate it to make sure that it can be properly
 * @written to the file. It makes sure that:
 * @    - There are no unrecognized markers
 * @    - There is no lists or tables nested in each other
 * @    - Tables and lists are formatted correctly
 * @    - There are no incomplete escapes
 * @    - There are no unclosed tables and lists
 * @description
 *
 * @param string: the string to validate
 * @type: struct CString
*/

void writer_validate_string(const char *string);

/*
 * @docgen: function
 * @brief: perform final steps of preprocessing and write the output
 * @name: dump_cstring
 *
 * @description
 * @This function will take the string generated by the preprocessor, and perform
 * @final modifications on the string like bolding text, expanding tables and lists,
 * @and more. Once this is all done, it is written to the file.
 * @description
 *
 * @param format: what documentation format are we writing?
 * @type: const char *
 *
 * @param string: the cstring to write
 * @type: struct CString
 *
 * @param output_path[LIBPATH_MAX_PATH + 1]: the output path to write to
 * @type: char
*/
void dump_cstring(const char *format, struct CString string, char output_path[LIBPATH_MAX_PATH + 1]);

#endif
