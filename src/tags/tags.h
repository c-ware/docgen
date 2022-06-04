#ifndef CWARE_DOCGEN_TAGS_H
#define CWARE_DOCGEN_TAGS_H

struct LibmatchCursor;

/* Limits 
 *
 * Limits here have a "+1" not for a NUL byte (although
 * that is added by users of these macros), but to signal
 * that to any parsing functions that a line is longer
 * than N characters. The actual maximum length of a buffer
 * is whatever the final value is, subtract 1.
*/
#define DOCGEN_TAG_LINE_LENGTH      1024 + 1
#define DOCGEN_TAG_NAME_LENGTH      32 + 1

/* Statuses that a tag can have */
#define DOCGEN_TAG_STATUS_SUCCESS   0   /* Tag parsed successfully */
#define DOCGEN_TAG_STATUS_EMPTY     1   /* Line is empty-- has no tags */
#define DOCGEN_TAG_STATUS_DONE      2   /* End of comment met; stop loop */
#define DOCGEN_TAG_STATUS_EOF       3   /* End of file reached before end of comment */
#define DOCGEN_TAG_STATUS_EOC       4   /* End of comment found on same line as tag */
#define DOCGEN_TAG_STATUS_FULL      5   /* Tag line could not fit in buffer */

/*
 * Represents a tag.
*/
struct DocgenTag {
    int status;
    char line[DOCGEN_TAG_LINE_LENGTH + 1];
};

/*
 * Stores the name of a tag.
*/
struct DocgenTagName {
    int status;
    char name[DOCGEN_TAG_NAME_LENGTH + 1];
};

struct DocgenTag docgen_tag_next(struct LibmatchCursor *cursor, const char *comment_start,
                                 const char *comment_end);
struct DocgenTagName docgen_tag_name(struct DocgenTag tag);

#endif
