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
 * File implementing main operations for cstring.
*/

#include <stdlib.h>
#include <string.h>

#include "cstring.h"

/* Memory focused operations */

struct CString cstring_init(const char *body) {
    int body_length = 0;
    struct CString cstring;

    liberror_is_null(cstring_init, body);

    body_length = strlen(body);
    cstring.length = body_length;
    cstring.capacity = body_length + 1;
    cstring.contents = malloc(body_length + 1);

    cstring.contents[0] = '\0';
    strncat(cstring.contents, body, body_length);
    cstring.contents[body_length] = '\0';

    return cstring;
}

void cstring_free(struct CString cstring) {
    liberror_is_null(cstring_free, cstring.contents);

    free(cstring.contents);
}

/* Addition based operations */
void cstring_concat(struct CString *cstring_a, struct CString cstring_b) {
    int index = 0;
    int new_length = 0;

    liberror_is_null(cstring_concat, cstring_a);
    liberror_is_null(cstring_concat, cstring_a->contents);
    liberror_is_null(cstring_concat, cstring_b.contents);

    liberror_is_negative(cstring_concat, cstring_a->length);
    liberror_is_negative(cstring_concat, cstring_b.length);

    liberror_is_negative(cstring_concat, cstring_a->capacity);
    liberror_is_negative(cstring_concat, cstring_b.capacity);

    /* Do not resize cstring_a if no change will be made */
    if(cstring_b.length == 0)
        return;

    new_length = cstring_a->length + cstring_b.length;

    /* Resize buffer first if the new length is above the old length.
     * This would be a factor in a situation where the string was
     * reset, and then we concatenate to the reset string. This is
     * essentially so we do not shrink the string. */
    if(new_length > cstring_a->length)
        cstring_a->contents = realloc(cstring_a->contents, new_length + 1);

    for(index = 0; index < cstring_b.length; index++) {
        cstring_a->contents[cstring_a->length + index] = cstring_b.contents[index];
    }

    /* Finalize cstring_a */
    cstring_a->contents[cstring_a->length + index] = '\0';
    cstring_a->length = new_length;
    cstring_a->capacity = new_length + 1;
}

void cstring_concats(struct CString *cstring, const char *string) {
    struct CString new_string;

    liberror_is_null(cstring_concats, cstring);
    liberror_is_null(cstring_concats, cstring_string(*cstring));
    liberror_is_null(cstring_concats, string);

    liberror_is_negative(cstring_concats, cstring->length);
    liberror_is_negative(cstring_concats, cstring->capacity);

    /* We cast away const here because the contents field of new_string,
     * which will be the string argument, is not modified in the context
     * of a concatenation. However, it is still not modified, so we cast
     * it away. That being said, casting away const here is.. less than
     * optimal. It will definitely raise a few eyebrows. */
    new_string.length = strlen(string);
    new_string.capacity = strlen(string) + 1;
    new_string.contents = (char *) string;

    cstring_concat(cstring, new_string);
}

/* Statistics related */

/* Removal based operations */
int cstring_strip(struct CString *cstring, struct CString target) {
    int index = 0;
    int strips = 0;

    liberror_is_null(cstring_strip, cstring);
    liberror_is_null(cstring_strip, cstring->contents);
    liberror_is_null(cstring_strip, target.contents);
    liberror_is_negative(cstring_strip, cstring->length);
    liberror_is_negative(cstring_strip, target.length);

    /* A target string cannot be removed from a string that
     * it is larger than */
    if(target.length > cstring->length)
        return 0;

    /* Keep stripping a string from the location its found.
     * This could be a bit simpler. Maybe having a cursor
     * and then simply shifting over everything after
     * index + target.length */
    while((index = cstring_find(*cstring, target)) != CSTRING_NOT_FOUND) {
        memmove(cstring->contents + index, cstring->contents + index + target.length, sizeof(char) * (cstring->length - target.length - index));
        cstring->contents[cstring->length - target.length] = '\0';
        cstring->length -= target.length;

        strips++;
    }

    return strips;
}

int cstring_strips(struct CString *cstring, const char *target) {
    struct CString cstring_target;

    liberror_is_null(cstring_strips, cstring);
    liberror_is_null(cstring_strips, target);
    liberror_is_negative(cstring_strips, cstring->length);

    cstring_target.length = strlen(target);
    cstring_target.capacity = strlen(target) + 1;
    cstring_target.contents = (char *) target;

    return cstring_strip(cstring, cstring_target);
}

/* Searching / condition based operations */
int cstring_find(struct CString haystack, struct CString needle) {
    int index = 0;

    liberror_is_negative(cstring_find, haystack.length);
    liberror_is_negative(cstring_find, needle.length);
    liberror_is_null(cstring_find, cstring_string(haystack));
    liberror_is_null(cstring_find, cstring_string(needle));

    for(index = 0; index < haystack.length; index++) {
        int cursor = 0;

        for(cursor = 0; cursor < needle.length; cursor++) {
            if(cstring_string(haystack)[index + cursor] == cstring_string(needle)[cursor])
                continue;

            /* Premature ending-- cursor will not be equal to cstring_b.length,
             * which will signal that this is NOT a match */
            break;
        }

        /* Signals an icorrect match */
        if(cursor != needle.length)
            continue;

        return index;
    }

    return CSTRING_NOT_FOUND;
}

int cstring_finds(struct CString haystack, const char *needle) {
    struct CString cstring;

    liberror_is_null(cstring_finds, cstring_string(haystack));
    liberror_is_null(cstring_finds, needle);
    liberror_is_negative(cstring_finds, haystack.length);

    cstring.length = strlen(needle);
    cstring.capacity = strlen(needle) + 1;
    cstring.contents = (char *) needle;

    return cstring_find(haystack, cstring);
}

int cstring_startswith(struct CString cstring, struct CString check) {
    int index = 0;

    liberror_is_null(cstring_startswith, cstring_string(cstring));
    liberror_is_null(cstring_startswith, cstring_string(check));
    liberror_is_negative(cstring_startswith, cstring.length);
    liberror_is_negative(cstring_startswith, check.length);

    /* Check cannot be inside of a larger string than it */
    if(check.length > cstring.length)
        return 0;

    for(index = 0; index < check.length; index++) {
        if(cstring_string(cstring)[index] == cstring_string(check)[index])
            continue;

        return 0;
    }

    return 1;
}

int cstring_startswiths(struct CString cstring, const char *check) {
    struct CString cstring_check;

    liberror_is_null(cstring_startswiths, cstring_string(cstring));
    liberror_is_null(cstring_startswiths, check);
    liberror_is_negative(cstring_startswiths, cstring.length);

    cstring_check.length = strlen(check);
    cstring_check.capacity = strlen(check) + 1;
    cstring_check.contents = (char *) check;

    return cstring_startswith(cstring, cstring_check);
}

int cstring_endswith(struct CString cstring, struct CString check) {
    int index = 0;
    int cursor = 0;

    liberror_is_null(cstring_endswith, cstring.contents);
    liberror_is_null(cstring_endswith, check.contents);
    liberror_is_negative(cstring_endswith, cstring.length);
    liberror_is_negative(cstring_endswith, check.length);

    /* Check cannot be inside of a larger string than it */
    if(check.length > cstring.length)
        return 0;

    for(index = cstring.length - check.length; index < cstring.length; index++) {
        if(cstring_string(cstring)[index] != cstring_string(check)[cursor])
            return 0;

        cursor++;
    }


    return 1;
}

int cstring_endswiths(struct CString cstring, const char *check) {
    struct CString cstring_check;

    liberror_is_null(cstring_endswiths, cstring_string(cstring));
    liberror_is_null(cstring_endswiths, check);
    liberror_is_negative(cstring_endswiths, cstring.length);

    cstring_check.length = strlen(check);
    cstring_check.capacity = strlen(check) + 1;
    cstring_check.contents = (char *) check;

    return cstring_endswith(cstring, cstring_check);
}

/* Less general purpose operations */
struct CString cstring_loadf(FILE *file) {
    int length = 0;
    struct CString cstring;

    liberror_is_null(cstring_loadf, file);

    fseek(file, 0, SEEK_END);
    length = ftell(file);

    /* Prepare the buffer and cstring */
    cstring.contents = malloc(sizeof(char) * (length + 1));
    cstring.contents[length] = '\0';
    cstring.length = length;
    cstring.capacity = length + 1;

    /* Load the file */
    rewind(file);
    fread(cstring.contents, 1, length, file);
    
    return cstring;
}

/* Misc. operations */
void cstring_reset(struct CString *cstring) {
    liberror_is_null(cstring_reset, cstring);
    liberror_is_null(cstring_reset, cstring->contents);
    liberror_is_negative(cstring_reset, cstring->length);

    cstring->length = 0;
    cstring->contents[0] = '\0';
}

struct CString cstring_slice(struct CString cstring, int start, int stop) {
    struct CString slice;

    liberror_is_null(cstring_slice, cstring.contents);
    liberror_is_negative(cstring_slice, cstring.length);
    liberror_is_negative(cstring_slice, start);
    liberror_is_negative(cstring_slice, stop);

    if(start > stop) {
        fprintf(stderr, "cstring_slice: start (%i) cannot be larger than stop (%i)\n",
                start, stop);
        exit(EXIT_FAILURE);
    }

    if(start > cstring.length) {
        fprintf(stderr, "cstring_slice: start (%i) outside of the bounds of the cstring length (%i)\n",
                start, cstring.length);
        exit(EXIT_FAILURE);
    }

    if(stop > cstring.length) {
        fprintf(stderr, "cstring_slice: stop (%i) outside of the bounds of the cstring length (%i)\n",
                stop, cstring.length);
        exit(EXIT_FAILURE);
    }

    /*
     * Slice a segment of a string into a new string.
     *
     * range: 0, 3
     * f o o b a r b a z
     * |---|
     *
     * range: 3, 6
     * f o o b a r b a z
     *       |---|
     *
     * length: (stop - start)
     * capacity: length
     * contents: cstring.contents + start
     *
     * Note: in the case of slicing, the capacity of the string will
     * be the same as the length, as there is NOT any modification
     * of the string during a slice, which means that
     * slice.contents[slice.length] will NOT necessarily be a NUL byte.
    */

    slice.contents = cstring.contents + start;
    slice.length = stop - start;
    slice.capacity = stop - start;

    return slice;
}
