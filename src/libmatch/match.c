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
 * Functions used in matching text in a string.
*/

#include <string.h>
#include <stdarg.h>

#include "libmatch.h"

int libmatch_expect(struct LibmatchCursor *cursor, int count,
                    const char *characters) {
    int character = -1;
    int matched = 0;

    while((character = libmatch_cursor_getch(cursor)) != LIBMATCH_EOF) {
        if(strchr(characters, character) == NULL) {
            _libmatch_pushback(cursor);

            break;
        }

        matched++;

        if(matched == count)
            return 1;
    }

    return 0;
}

int libmatch_atleast(struct LibmatchCursor *cursor, int count,
                     const char *characters) {
    int character = -1;
    int matched = 0;

    while((character = libmatch_cursor_getch(cursor)) != LIBMATCH_EOF) {
        if(strchr(characters, character) == NULL) {
            _libmatch_pushback(cursor);

            break;
        }

        matched++;

    }

    if(matched >= count)
        return 1;

    return 0;
}

int libmatch_string_expect(struct LibmatchCursor *cursor, const char *string) {
    int matched = 0;
    int character = -1;
    int string_cursor = 0;

    while((character = libmatch_cursor_getch(cursor)) != LIBMATCH_EOF) {
        if(string[string_cursor] == character) {
            string_cursor++;

            if(string[string_cursor] == '\0')
                return 1;

            continue;
        }

        _libmatch_pushback(cursor);

        break;
    }

    return 0;
}

int libmatch_strings_expect(struct LibmatchCursor *cursor, ...) {
    va_list strings;
    const char *string = NULL;
    int index = 0;
    int cursor_position = cursor->cursor;
    int cursor_line = cursor->line;
    int cursor_character = cursor->character;

    va_start(strings, cursor);

    while((string = va_arg(strings, char *)) != NULL) {

        /* Reset cursor */
        if(libmatch_string_expect(cursor, string) == 0) {
            cursor->cursor = cursor_position;
            cursor->line = cursor_line;
            cursor->character = cursor_character;
            index++;

            continue;
        }

        return index;
    }

    return -1;
}

int libmatch_until(struct LibmatchCursor *cursor, const char *characters) {
    int matched = 0;
    int character = -1;

    while((character = libmatch_cursor_getch(cursor)) != LIBMATCH_EOF) {
        if(strchr(characters, character) != NULL) {
            _libmatch_pushback(cursor);

            break;
        }


        matched++;
    }

    return matched;
}

int libmatch_expect_next(struct LibmatchCursor *cursor,
                         const char *characters) {
    int next = libmatch_cursor_getch(cursor);

    if(next == EOF)
        return 0;

    if(strchr(characters, next) == NULL) {
        _libmatch_pushback(cursor);

        return 0;
    }

    return 1;
}

int libmatch_next_line(struct LibmatchCursor *cursor) {
    int skipped = 0;
    int character = -1;

    if(cursor->buffer[cursor->cursor] == '\n') {
        libmatch_cursor_getch(cursor);

        return 0;
    }

    while((character = libmatch_cursor_getch(cursor)) != LIBMATCH_EOF) {
        if(character == '\n')
            break;

        skipped++;
    }

    return skipped;
}
