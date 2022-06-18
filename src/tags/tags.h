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
