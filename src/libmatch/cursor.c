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
 * Cursor-related functions.
*/

#include <stdlib.h>

#include "libmatch.h"

struct LibmatchCursor libmatch_cursor_init(char *buffer, int length) {
    struct LibmatchCursor new_cursor = {LIBMATCH_CURSOR_NULL};

    new_cursor.cursor = 0;
    new_cursor.length = length;
    new_cursor.buffer = buffer;

    return new_cursor;
}

struct LibmatchCursor libmatch_cursor_from_stream(FILE *stream) {
    struct LibmatchCursor new_cursor = {LIBMATCH_CURSOR_NULL};
    char *new_buffer = malloc(sizeof(char) * LIBMATCH_INITIAL_BUFFER_SIZE);

    int length = 0;
    int capacity = LIBMATCH_INITIAL_BUFFER_SIZE;
    char character = -1;

    /* Keep reading the linitial size until an EOF is found. */
    while((character = fgetc(stream)) != EOF) {
        if(length == capacity) {
            capacity += LIBMATCH_BUFFER_GROWTH;
            new_buffer = realloc(new_buffer, sizeof(char) * capacity);
        }


        new_buffer[length] = character;
        length++;
    }

    new_cursor.buffer = new_buffer;
    new_cursor.length = length;

    return new_cursor;
}

int libmatch_cursor_getch(struct LibmatchCursor *cursor) {
    int character = -1;

    /* Return EOF */
    if(cursor->cursor == cursor->length)
        return LIBMATCH_EOF;

    character = cursor->buffer[cursor->cursor];
    cursor->cursor++;

    /* Handle coordinates */
    if(character == '\n') {
        cursor->line++;
        cursor->character = 0;
    }

    cursor->character++;

    return character;
}

void libmatch_cursor_ungetch(struct LibmatchCursor *cursor) {
    if(cursor->cursor == 0)
        return;

    cursor->cursor--;

    /* Decrement line and character */
    if(cursor->buffer[cursor->cursor] == '\n') {
        int index = cursor->cursor - 1;
        int length = 0;

        cursor->line--;

        /* Determine the length of this line */
        while(index >= 0) {
            if(cursor->buffer[index] == '\n') {
                cursor->character = length;

                break;
            }
            
            index--; 
            length++;
        }

        return;
    }

    cursor->character--;
}

int libmatch_cursor_unwind(struct LibmatchCursor *cursor, int distance) {
    int unwound = 0;

    while((cursor->cursor > 0) && (unwound < distance)) {
        libmatch_cursor_ungetch(cursor);
        unwound++;
    }

    return unwound;
}

void libmatch_cursor_enable_pushback(struct LibmatchCursor *cursor) {
    cursor->pushback = 1;
}

void libmatch_cursor_disable_pushback(struct LibmatchCursor *cursor) {
    cursor->pushback = 0;
}

void libmatch_cursor_free(struct LibmatchCursor *cursor) {
    free(cursor->buffer);
}
