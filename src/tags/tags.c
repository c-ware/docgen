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
