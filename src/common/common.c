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
    liberror_is_negative(docgen_extract_field_block, length);

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

    do_padding(struct DocgenStructureField field, int longest, int depth,
                       FILE *location) {
    int padding = 0;
    int padding_index = 0;
    int field_length = 0;

    liberror_is_null(docgen_extract_field_block, location);
    liberror_is_negative(docgen_extract_field_block, longest);
    liberror_is_negative(docgen_extract_field_block, depth);

    /* Add padding before the description */
    padding = (longest - docgen_get_field_length(field, depth));

    for(padding_index = 0; padding_index < padding; padding_index++) {
        fprintf(location, "%c", ' ');
    }
}
