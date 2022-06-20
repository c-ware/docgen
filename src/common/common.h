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

#ifndef CWARE_DOCGEN_COMMON_H
#define CWARE_DOCGEN_COMMON_H

#define docgen_get_field_length(field, depth) \
    (((depth + 1) * DOCGEN_INDENTATION) + strlen((field).type) + strlen((field).name) + 1 + 1)

struct LibmatchCursor;
struct DocgenArguments;
struct DocgenStructureField;

/* Error handling */
#define tag_error(new_tag, cursor)                                                                         \
    switch((new_tag)->status) {                                                                            \
        case DOCGEN_TAG_STATUS_EMPTY:                                                                      \
            continue;                                                                                      \
        case DOCGEN_TAG_STATUS_DONE:                                                                       \
            goto break_loop;                                                                               \
        case DOCGEN_TAG_STATUS_EOF:                                                                        \
            fprintf(stderr, "docgen: file ended without any closing comment (%s:%i)\n",                    \
                    __FILE__, __LINE__);                                                                   \
                                                                                                           \
            exit(EXIT_FAILURE);                                                                            \
        case DOCGEN_TAG_STATUS_EOC:                                                                        \
            fprintf(stderr, "docgen: comment ends on the same line as a tag (line %i) (%s:%i)\n",          \
                    (cursor)->line, __FILE__, __LINE__);                                                   \
                                                                                                           \
            exit(EXIT_FAILURE);                                                                            \
        case DOCGEN_TAG_STATUS_FULL:                                                                       \
            fprintf(stderr, "docgen: line %i could not fit in line buffer. maximum size of %i (%s:%i)\n",  \
                    (cursor)->line, DOCGEN_TAG_LINE_LENGTH - 1, __FILE__, __LINE__);                       \
            exit(EXIT_FAILURE);                                                                            \
    }                                                                                                      \

#define next_tag_error(type_tag, cursor, first, second)                                                         \
    switch((type_tag)->status) {                                                                                \
        case DOCGEN_TAG_STATUS_EMPTY:                                                                           \
            fprintf(stderr, "docgen: %s and %s tags on line %i cannot have any lines between them (%s:%i)\n",   \
                    (first), (second), (cursor)->line, __FILE__, __LINE__);                                     \
                                                                                                                \
            exit(EXIT_FAILURE);                                                                                 \
        case DOCGEN_TAG_STATUS_DONE:                                                                            \
            fprintf(stderr, "docgen: comment ended at line %i before type could be determined (%s:%i)\n",       \
                    (cursor)->line, __FILE__, __LINE__);                                                        \
                                                                                                                \
            exit(EXIT_FAILURE);                                                                                 \
        case DOCGEN_TAG_STATUS_EOF:                                                                             \
            fprintf(stderr, "docgen: file ended without any closing comment (%s:%i)\n",                         \
                    __FILE__, __LINE__);                                                                        \
                                                                                                                \
            exit(EXIT_FAILURE);                                                                                 \
        case DOCGEN_TAG_STATUS_EOC:                                                                             \
            fprintf(stderr, "docgen: comment ends on the same line as a type tag (line %i) (%s:%i)\n",          \
                    (cursor)->line, __FILE__, __LINE__);                                                        \
                                                                                                                \
            exit(EXIT_FAILURE);                                                                                 \
        case DOCGEN_TAG_STATUS_FULL:                                                                            \
            fprintf(stderr, "docgen: line %i could not fit in line buffer. maximum size of %i (%s:%i)\n",       \
                    (cursor)->line, DOCGEN_TAG_LINE_LENGTH - 1, __FILE__, __LINE__);                            \
            exit(EXIT_FAILURE);                                                                                 \
    }                                                                                                           \

void docgen_extract_type(struct LibmatchCursor *cursor, char *buffer, int length);
void docgen_parse_comment(struct LibmatchCursor *cursor);
int docgen_comment_is_type(struct LibmatchCursor *cursor, const char *comment_start,
                           const char *commend_end, const char *type);
void docgen_extract_block(struct LibmatchCursor *cursor, char *buffer,
                          int length, const char *bound);
const char *docgen_get_comment_start(struct DocgenArguments arguments);
const char *docgen_get_comment_end(struct DocgenArguments arguments);

void docgen_create_file_path(struct DocgenArguments arguments, const char *name,
                             char *buffer, int length);

void docgen_extract_field_line(const char *tag_name, char *buffer, int buffer_length,
                               int line_number, char *tag_line);

void docgen_extract_field_block(const char *tag_name, int length, struct LibmatchCursor *cursor,
                                char *read, char *buffer);

void docgen_extract_field_line_arg(const char *tag_name, char *read, int argument_length,
                                   char *argument_buffer, int description_length,
                                   char *description_buffer, int line);

struct Reference docgen_extract_reference(struct LibmatchCursor *cursor,
                                          struct DocgenTag new_tag);

void docgen_do_padding(struct DocgenStructureField field, int longest, int depth,
                       FILE *location);

void field_line_error_check(const char *line, int line_number);

void field_line_arg_error_check(const char *line, int line_number);

#endif
