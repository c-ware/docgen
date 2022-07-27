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
 * This file contains common logic that might be useful to all of the postprocessor
 * implementations.
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../docgen.h"
#include "writer/writer.h"

/*
 * Error checking logic
*/

void unrecognized_markers(const char *string) {
    int cindex = 0;
    int length = strlen(string);

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
}

void no_nested_elements(const char *string) {
    int cindex = 0;
    int in_list = 0;
    int in_table = 0;
    int length = strlen(string);

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
}

void no_unclosed_elements(const char *string) {
    int cindex = 0;
    int in_list = 0;
    int in_table = 0;
    int length = strlen(string);

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

    /* Reset these so that we can reliably tell which are not closed
     * based off which are set to 1 when the loop ends. */
    in_list = 0;
    in_table = 0;

    if(in_list == 1) {
        fprintf(stderr, "%s", "docgen: unclosed list inside of output\n");    
        exit(EXIT_FAILURE);
    }

    if(in_table == 1) {
        fprintf(stderr, "%s", "docgen: unclosed table inside of output\n");    
        exit(EXIT_FAILURE);
    }
}
