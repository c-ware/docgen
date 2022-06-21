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


/*
 * @docgen: function
 * @brief: determine if the type of the comment is expected
 * @name: docgen_comment_is_type
 *
 * @include: common.h
 *
 * @description
 * @Given the cursor's position in the comment, determine if the comment
 * @has a type that matches the expected type
 * @description
 *
 * @param cursor: the cursor to read the comment type from
 * @type: struct LibmatchCursor *
 *
 * @param comment_start: the start of comment token
 * @type: const char *
 *
 * @param comment_end: the end of comment token
 * @type: const char *
 *
 * @param type: the expected type
 * @type: const char *
 *
 * @return: whether or not the comment matches
 * @type: int
*/
int docgen_comment_is_type(struct LibmatchCursor *cursor, const char *comment_start,
                           const char *comment_end, const char *type);

/*
 * @docgen: function
 * @brief: determine the characters that qualify a start of comment
 * @name: docgen_get_comment_start
 *
 * @include: common.h
 *
 * @description
 * @This function will, given the command line arguments to docgen, determine
 * @what the proper start of block comment token is. This function really only 
 * @exists to assist in future-proofing docgen incase of any other language
 * @support we may want to add in the future, but it is also a bit cleaner.
 * @description
 *
 * @param arguments: the arguments to docgen
 * @type: struct DocgenArguments
 *
 * @return: the start of comment token
 * @type: const char *
*/
const char *docgen_get_comment_start(struct DocgenArguments arguments);

/*
 * @docgen: function
 * @brief: determine the characters that qualify an end of comment
 * @name: docgen_get_comment_end
 *
 * @include: common.h
 *
 * @description
 * @This function will, given the command line arguments to docgen, determine
 * @what the proper end of block comment token is. This function really only 
 * @exists to assist in future-proofing docgen incase of any other language
 * @support we may want to add in the future, but it is also a bit cleaner.
 * @description
 *
 * @param arguments: the arguments to docgen
 * @type: struct DocgenArguments
 *
 * @return: the end of comment token
 * @type: const char *
*/
const char *docgen_get_comment_end(struct DocgenArguments arguments);

/*
 * @docgen: function
 * @brief: construct a path to a file
 * @name: docgen_create_file_path
 *
 * @include: common.h
 *
 * @description
 * @This function exists for two reasons:
 * @    - Reduce the boilerplate for creating file paths, which may or may
 * @      may not have portability differences
 * @    - Future proof path-dependant code (which we all know is a problem)
 * @
 * @The final path that is produced by this functionw will be NUL terminated.
 * @description
 *
 * @error: name is NULL
 * @error: buffer is NULL
 * @error: length is negative
 *
 * @param arguments: the parsed command line arguments given to docgen
 * @type: struct DocgenArguments
 *
 * @param name: the name of the file to create
 * @type: const char *
 *
 * @param buffer: the buffer to write the path to
 * @type: char *
 *
 * @param length: the maximum length of the buffer
 * @type: int
*/
void docgen_create_file_path(struct DocgenArguments arguments, const char *name,
                             char *buffer, int length);

/*
 * @docgen: function
 * @brief: extract a field from a line
 * @name: docgen_extract_field_line
 *
 * @include: common.h
 *
 * @description
 * @This function will extract a field from a line into a buffer. The buffer
 * @will be NUL terminated, and the expected input is something of the form of
 * @@<TAG_NAME>: <FIELD>
 * @description
 *
 * @error: tag_name is NULL
 * @error: buffer is NULL
 * @error: tag_line is NULL
 * @error: buffer_length is negative
 * @error: line_number is negative
 *
 * @param tag_name: the name of the tag that is being parsed
 * @type: const char *
 *
 * @param buffer: the buffer to write the field into
 * @type: char *
 *
 * @param buffer_length: the maximum length of the buffer
 * @type: int
 *
 * @param line_number: the line number of the cursor
 * @type: int
 *
 * @param tag_line: the line to extract data from
 * @type: char *
*/
void docgen_extract_field_line(const char *tag_name, char *buffer, int buffer_length,
                               int line_number, char *tag_line);

/*
 * @docgen: function
 * @brief: extract lines between two instances of the same tag
 * @name: docgen_extract_field_block
 *
 * @include: common.h
 *
 * @description
 * @This function will extract all lines between two instances of the same
 * @tag, and write it into a buffer. Used internally for tags like descriptions,
 * @notes, etc. Each in between the two tags must have a '@' appear somewhere,
 * @and after that character the 'line' will be considered to start. As such,
 * @the '@' will not appear in the final buffer.
 * @description
 *
 * @notes
 * @This function has not yet been 'prettified,' either in parameter naming
 * @or internal structure. Also, the cursor parameter will most likely be
 * @ripped from the function arguments.
 * @notes
 *
 * @error: tag_name is NULL
 * @error: tag_line is NULL
 * @error: cursor is NULL
 * @error: buffer is NULL
 * @error: length is negative
 *
 * @param tag_name: the name of the tag that is being parsed
 * @type: const char *
 *
 * @param buffer: the buffer to write the block into
 * @type: char *
 *
 * @param length: the length of the block buffer
 * @type: int
 *
 * @param cursor: the cursor to use
 * @type: struct LibmatchCursor *
 *
 * @param read: the line that the first tag is on
 * @type: char *
*/
void docgen_extract_field_block(const char *tag_name, char *buffer, int length,
                                struct LibmatchCursor *cursor, char *tag_line);

/*
 * @docgen: function
 * @brief: extract an argument and field from a line
 * @name: docgen_extract_field_line_arg
 *
 * @include: common.h
 *
 * @description
 * @Given a line, extract the argument and field from the line-- or, in a more
 * @visual way (<[A-Z]+> are place holders):
 * @
 * @@<TAG_NAME> <ARGUMENT>: <FIELD>
 * @
 * @The final buffers will be terminated by a NUL byte, and the buffers should
 * @have an actual size of LENGTH + 1 to make space for the NUL byte, although
 * @this function should just have LENGTH as the actual size.
 * @description
 *
 * @notes
 * @A potential future development is changing the parameter 'argument_{buffer,length}'
 * @to field_{buffer,length} to keep things more consistent. This is not remotely
 * @a breaking API change, but it is a rather small thing.
 * @notes
 *
 * @error: tag_name is NULL
 * @error: argument_buffer is NULL
 * @error: description_buffer is NULL
 * @error: tag_line is NULL
 * @error: argument_length is negative
 * @error: description_length is negative
 * @error: line_length is negative
 *
 * @param tag_name: the name of the tag that is being parsed
 * @type: const char *
 *
 * @param argument_buffer: the buffer to write the argument to
 * @type: char *
 *
 * @param argument_length: the maximum length of the argument buffer
 * @type: int
 *
 * @param description_buffer: the buffer to write the description to
 * @type: char *
 *
 * @param description_length: the maximum length of the description buffer
 * @type: int
 *
 * @param line_number: the line number the cursor is on
 * @type: int
 *
 * @param tag_line: the line to extract data from
 * @type: char *
*/
void docgen_extract_field_line_arg(const char *tag_name, char *argument_buffer,
                                   int argument_length, char *description_buffer,
                                   int description_length, int line_number, char *tag_line);

/*
 * @docgen: function
 * @brief: extract a reference from a line
 * @name: docgen_extract_reference
 *
 * @include: common.h
 *
 * @description
 * @Given a line that should contain a reference, attempt to extract it
 * @and produce a structure that has the information of the reference.
 * @description
 *
 * @notes
 * @The cursor parameter is HIGHLY likely to be removed in the future, and
 * @replaced with a parameter that is just the line that the cursor is on,
 * @as that the only thing that is accessed in it.
 * @notes
 *
 * @error: cursor is NULL
 *
 * @param cursor: the cursor (potentially deprecated-- see notes)
 * @type: struct LibmatchCursor *
 *
 * @param new_tag: the tag to extract the reference from
 * @type: struct DocgenTag
 *
 * @return: a parsed reference
 * @type: struct Reference
*/
struct Reference docgen_extract_reference(struct LibmatchCursor *cursor,
                                          struct DocgenTag new_tag);

/*
 * @docgen: function
 * @brief: write a structure field to a file with padding
 * @name: docgen_do_padding
 *
 * @include: common.h
 *
 * @description
 * @Writes a structure field to a file location with added padding for the
 * @offset from the start of the line, and for the comment. This function
 * @is ONLY called after all structure fields have been parsed, and as such
 * @the 'longest' parameter can be known before the function is called.
 * @description
 *
 * @error: longest is negative
 * @error: depth is negative
 * @error: location is NULL
 *
 * @param field: the structure field to write
 * @type: struct DocgenStructureField
 * 
 * @param longest: the longest structure field that was parsed
 * @type: int
 *
 * @param depth: how deep is this structure field in a nested structure
 * @type: int
 * 
 * @param location: the file to write the field to
 * @type: FILE *
*/
void docgen_do_padding(struct DocgenStructureField field, int longest, int depth,
                       FILE *location);

/*
 * @docgen: function
 * @brief: validate a field line
 * @type: field_line_error_check
 *
 * @include: common.h
 *
 * @description
 * @Performs error checks to make sure that a field line is
 * @syntactically correct so that the main parser can extract the
 * @contents without worrying about errors
 * @description
 *
 * @error: line is NULL
 * @error: line_number is negative
 *
 * @param line: the parsed line to error check
 * @type: const char *
 *
 * @param line_number: the line number of the parser
 * @type: int
*/
void field_line_error_check(const char *line, int line_number);

/*
 * @docgen: function
 * @brief: validate a field + arugment line
 * @name: field_line_arg_error_check
 *
 * @include: common.h
 *
 * @description
 * @Performs error checks to make sure that a field + argument line
 * @is syntactically correct so that the main parser can extract the
 * @contents without worrying about errors
 * @description
 *
 * @error: line is NULL
 * @error: line_number is negative
 *
 * @param line: the parsed line to error check
 * @type: const char *
 *
 * @param line_number: the line number of the parser
 * @type: int
*/
void field_line_arg_error_check(const char *line, int line_number);

/*
 * @docgen: function
 * @brief: perform error checking on a line in a block
 * @name: block_error_check
 *
 * @include: common.h
 *
 * @description
 * @This function will enforce a specific format on on each line inside of
 * @a block. This function itself does NOT traverse each line itself, but
 * @rather relies on the caller to extract the next line.
 * @description
 *
 * @notes
 * @This function is currently unused, although it is my intention to
 * @use it sometime in the future.
 * @notes
 *
 * @error: line is NULL
 * @error: line_number is negative
 *
 * @param line: the parsed line to error check
 * @type: const char *
 *
 * @param line_number: the line number of the parser
 * @type: int
*/
void block_error_check(const char *line, int line_number);

/*
 * From what I can tell, this functions are not used anywhere, so I have decided
 * to deprecate them for now, but they might have use somewhere else and I just
 * could not find it.
 *
 * void docgen_parse_comment(struct LibmatchCursor *cursor);
 * void docgen_extract_type(struct LibmatchCursor *cursor, char *buffer, int length);
 * void docgen_extract_block(struct LibmatchCursor *cursor, char *buffer, int length, const char *bound);
*/

#endif
