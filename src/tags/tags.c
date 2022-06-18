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
 * This file contains functions relating to tags, tag extraction, and tag
 * handling.
*/

#include <string.h>

#include "tags.h"
#include "../docgen.h"

struct DocgenTag docgen_tag_next(struct LibmatchCursor *cursor, const char *comment_start,
                                 const char *comment_end) {
    int written = 0;
    struct DocgenTag new_tag;

    /*
     * ATTENTION TO ANYONE MODIFYING THIS FUNCTION:
     *
     * The order of these conditions MATTER! At least,
     * one does. Checking for the end of the comment and then
     * an EOF should be top priority.
    */

    /* Read a line */
    memset(&new_tag, 0, sizeof(struct DocgenTag));
    written = libmatch_read_until(cursor, new_tag.line, DOCGEN_TAG_LINE_LENGTH, "\n");

    /* End of comment; must be on its own line, or if
     * the `comment_end` is empty, then assume this is in
     * single-comment mode, and stop if the line does not
     * start with the start of comment symbol. */
    /*if(comment_end[0] == '\0' && strncmp(comment_start, new_tag.line, strlen(comment_start) != 0)) {*/
    if(strstr(new_tag.line, comment_end) != NULL && strchr(new_tag.line, '@') == NULL) {
        new_tag.status = DOCGEN_TAG_STATUS_DONE;

        return new_tag;
    }

    if(strstr(new_tag.line, "*/") != NULL && strchr(new_tag.line, '@') == NULL) {
        new_tag.status = DOCGEN_TAG_STATUS_DONE;

        return new_tag;
    }

    /* EOF found before the end of the comment.
     * This cannot really happen in a situation where
     * we are in single-comment mode, so we must check
     * beforehand.*/
    /*if(strcmp(comment_end, "") != 0 && cursor->cursor == cursor->length) {*/
    if(cursor->cursor == cursor->length) {
        new_tag.status = DOCGEN_TAG_STATUS_EOF;

        return new_tag;
    }

    /* Line is too long to fit in buffer */
    if(written == DOCGEN_TAG_LINE_LENGTH) {
        new_tag.status = DOCGEN_TAG_STATUS_FULL;

        return new_tag;
    }

    /* Line is empty-- it has no tag */
    if(strchr(new_tag.line, '@') == NULL) {
        new_tag.status = DOCGEN_TAG_STATUS_EMPTY;

        return new_tag;
    }

    /* Line has a tag, but there is an end of comment on it.
     * This cannot happen in single-comment mode, so we must
     * check beforehand. */
    /*if(strcmp(comment_end, "") != 0 && strstr(new_tag.line, comment_end) != NULL) {*/
    if(strstr(new_tag.line, comment_end) != NULL) {
        new_tag.status = DOCGEN_TAG_STATUS_EOC;

        return new_tag;
    }

    new_tag.status = DOCGEN_TAG_STATUS_SUCCESS;

    return new_tag;
}

struct DocgenTagName docgen_tag_name(struct DocgenTag tag) {
    int written = 0;
    struct DocgenTagName new_tag_name;
    struct LibmatchCursor tag_cursor = libmatch_cursor_init(tag.line, DOCGEN_TAG_LINE_LENGTH);

    memset(&new_tag_name, 0, sizeof(struct DocgenTagName));
    libmatch_until(&tag_cursor, "@");
    written = libmatch_read_until(&tag_cursor, new_tag_name.name,
                                  DOCGEN_TAG_NAME_LENGTH, " :\n");

    /* Full buffer; cannot hold the entire line */
    if(written == DOCGEN_TAG_NAME_LENGTH) {
        new_tag_name.status = DOCGEN_TAG_STATUS_FULL;

        return new_tag_name;
    }

    new_tag_name.status = DOCGEN_TAG_STATUS_SUCCESS;

    return new_tag_name;
}
