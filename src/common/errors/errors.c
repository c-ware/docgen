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
 * This file implements common error handling routines, like getting the line
 * number.
*/

#include "errors.h"

#include "../../docgen.h"

/* 
 * Scan the stdin body until the index location to see which line
 * the cursor is on. The search will stop when it goes beyond the
 * given index.
*/
int common_errors_get_line(struct CString body, int index) {
    int line = 0;
    int cursor = 0;

    LIBERROR_IS_NULL(body.contents);
    LIBERROR_IS_NEGATIVE(index);
    LIBERROR_IS_NEGATIVE(body.length);
    LIBERROR_IS_NEGATIVE(body.capacity);
    LIBERROR_IS_VALUE(body.length, 0);
    LIBERROR_IS_VALUE(body.capacity, 0);
    LIBERROR_OUT_OF_BOUNDS(index, body.length);

    /* We can do this rather than the actual length since we verify that the
     * index to stop at is within the bounds of the length.  */
    while(cursor < index) {
        int character = 0;

        LIBERROR_OUT_OF_BOUNDS(cursor, body.length);
        character = body.contents[cursor];

        if(character != '\n') {
            cursor++;

            continue;
        }

        line++;
        cursor++;
    }

    return line;
}
