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
 * This file is used to extract functions from a buffer.
*/

#include "functions.h"

struct DocgenFunction docgen_parse_function_comment(struct LibmatchCursor *cursor,
                                                    const char *comment_start,
                                                    const char *comment_end) {
    struct DocgenFunction new_function;
    
    memset(&new_function, 0, sizeof(struct DocgenFunction));

    /* Initialize function container */
    new_function.errors = carray_init(new_function.errors, ERROR);
    new_function.parameters = carray_init(new_function.parameters, PARAMETER);
    new_function.references = carray_init(new_function.references, REFERENCE);
    new_function.inclusions = carray_init(new_function.inclusions, INCLUDE);

    while(1) {
        struct DocgenTagName tag_name;
        struct DocgenTag new_tag;

        memset(&tag_name, 0, sizeof(struct DocgenTagName));
        memset(&new_tag, 0, sizeof(struct DocgenTag));

        new_tag = docgen_tag_next(cursor, comment_start, comment_end);

        /* Handle errors from the tag parser */
        tag_error(&new_tag, cursor);

        /* Extract the tag name */
        tag_name = docgen_tag_name(new_tag);

        /* Handle errors from the tag name extraction */
        switch(tag_name.status) {
            case DOCGEN_TAG_STATUS_FULL:
                fprintf(stderr, "docgen: name of tag on line %i is too long. maximum size of %i\n",
                        cursor->line, DOCGEN_TAG_NAME_LENGTH - 1);
                exit(EXIT_FAILURE);
        }

        /* Handle the tags */
        if(strcmp(tag_name.name, "name") == 0)
            docgen_extract_field_line("name", new_function.name, DOCGEN_FUNCTION_NAME_LENGTH, cursor->line,
                                      new_tag.line);

        else if(strcmp(tag_name.name, "brief") == 0)
            docgen_extract_field_line("brief", new_function.brief, DOCGEN_FUNCTION_BRIEF_LENGTH,
                                      cursor->line, new_tag.line);

        else if(strcmp(tag_name.name, "description") == 0)
            docgen_extract_field_block("description", new_function.description,
                                       DOCGEN_FUNCTION_DESCRIPTION_LENGTH, cursor, new_tag.line);

        else if(strcmp(tag_name.name, "notes") == 0)
            docgen_extract_field_block("notes", new_function.notes,
                                       DOCGEN_FUNCTION_NOTES_LENGTH, cursor, new_tag.line);

        else if(strcmp(tag_name.name, "example") == 0)
            docgen_extract_field_block("example", new_function.example,
                                       DOCGEN_FUNCTION_EXAMPLE_LENGTH, cursor, new_tag.line);

        else if(strcmp(tag_name.name, "include") == 0) {
            struct Inclusion new_inclusion;

            memset(&new_inclusion, 0, sizeof(struct Inclusion));
            new_inclusion.type = DOCGEN_INCLUSION_LOCAL;

            docgen_extract_field_line("include", new_inclusion.path, DOCGEN_INCLUSION_LENGTH,
                                      cursor->line, new_tag.line);

            carray_append(new_function.inclusions, new_inclusion, INCLUDE);
        }

        else if(strcmp(tag_name.name, "isystem") == 0) {
            struct Inclusion new_inclusion;

            memset(&new_inclusion, 0, sizeof(struct Inclusion));
            new_inclusion.type = DOCGEN_INCLUSION_SYSTEM;

            docgen_extract_field_line("isystem", new_inclusion.path, DOCGEN_INCLUSION_LENGTH,
                                      cursor->line, new_tag.line);

            carray_append(new_function.inclusions, new_inclusion, INCLUDE);
        }

        else if(strcmp(tag_name.name, "error") == 0) {
            struct DocgenFunctionError new_error;

            memset(&new_error, 0, sizeof(struct DocgenFunctionError));
            docgen_extract_field_line("error", new_error.description, DOCGEN_ERROR_DESCRIPTION_LENGTH,
                                      cursor->line, new_tag.line);

            carray_append(new_function.errors, new_error, ERROR);
        }

        else if(strcmp(tag_name.name, "reference") == 0) {
            struct Reference new_reference;

            memset(&new_reference, 0, sizeof(struct Reference));
            new_reference = docgen_extract_reference(cursor, new_tag);

            carray_append(new_function.references, new_reference, REFERENCE);
        }

        else if(strcmp(tag_name.name, "param") == 0) {
            struct DocgenTag type_tag;
            struct DocgenTagName type_tag_name;
            struct DocgenFunctionParameter new_parameter;

            memset(&type_tag, 0, sizeof(struct DocgenTag));
            memset(&type_tag_name, 0, sizeof(struct DocgenTagName));
            memset(&new_parameter, 0, sizeof(struct DocgenFunctionParameter));

            docgen_extract_field_line_arg("param", new_parameter.name, DOCGEN_PARAMETER_NAME_LENGTH,
                                          new_parameter.description, DOCGEN_PARAMETER_DESCRIPTION_LENGTH,
                                          cursor->line, new_tag.line);

            type_tag = docgen_tag_next(cursor, comment_start, comment_end);

            /* Next line must be a type-- cannot even be emppty. */
            next_tag_error(&type_tag, cursor, "parameter", "type");

            /* Extract the tag name */
            tag_name = docgen_tag_name(type_tag);

            /* Handle errors from the tag name extraction */
            switch(tag_name.status) {
                case DOCGEN_TAG_STATUS_FULL:
                    fprintf(stderr, "docgen: name of tag on line %i is too long. maximum size of %i\n",
                            cursor->line, DOCGEN_TAG_NAME_LENGTH - 1);
                    exit(EXIT_FAILURE);
            }

            /* Parse the type */
            docgen_extract_field_line("type", new_parameter.type, DOCGEN_TYPE_LENGTH, cursor->line, type_tag.line);
            carray_append(new_function.parameters, new_parameter, PARAMETER);
        }

        else if(strcmp(tag_name.name, "return") == 0) {
            struct DocgenTag type_tag;
            struct DocgenTagName return_tag_name;

            memset(&type_tag, 0, sizeof(struct DocgenTag));
            memset(&return_tag_name, 0, sizeof(struct DocgenTagName));
            docgen_extract_field_line("return", new_function.return_data.return_value,
                                      DOCGEN_FUNCTION_RETURN_LENGTH, cursor->line,
                                      new_tag.line);

            type_tag = docgen_tag_next(cursor, comment_start, comment_end);

            /* Next line must be a type-- cannot even be emppty. */
            switch(type_tag.status) {
                case DOCGEN_TAG_STATUS_EMPTY:
                    fprintf(stderr, "docgen: return and type tags on line %i cannot have any lines between them\n",
                            cursor->line);

                    exit(EXIT_FAILURE);
                case DOCGEN_TAG_STATUS_DONE:
                    fprintf(stderr, "docgen: comment ended at line %i before type could be determined\n",
                            cursor->line);

                    exit(EXIT_FAILURE);
                case DOCGEN_TAG_STATUS_EOF:
                    fprintf(stderr, "%s", "docgen: file ended without any closing comment\n");

                    exit(EXIT_FAILURE);
                case DOCGEN_TAG_STATUS_EOC:
                    fprintf(stderr, "docgen: comment ends on the same line as a type tag (line %i)\n", cursor->line);

                    exit(EXIT_FAILURE);
                case DOCGEN_TAG_STATUS_FULL:
                    fprintf(stderr, "docgen: line %i could not fit in line buffer. maximum size of %i\n",
                            cursor->line, DOCGEN_TAG_LINE_LENGTH - 1);
                    exit(EXIT_FAILURE);
            }

            /* Extract the tag name */
            tag_name = docgen_tag_name(type_tag);

            /* Handle errors from the tag name extraction */
            switch(tag_name.status) {
                case DOCGEN_TAG_STATUS_FULL:
                    fprintf(stderr, "docgen: name of tag on line %i is too long. maximum size of %i\n",
                            cursor->line, DOCGEN_TAG_NAME_LENGTH - 1);
                    exit(EXIT_FAILURE);
            }

            /* Parse the type */
            docgen_extract_field_line("type", new_function.return_data.return_type, DOCGEN_TYPE_LENGTH,
                                      cursor->line, type_tag.line);
        } else {
            fprintf(stderr, "docgen: unknown tag '%s' in function extractor on line %i (%s)\n",
                    tag_name.name, cursor->line, new_tag.line);
            exit(EXIT_FAILURE);
        }
    }

break_loop:

    return new_function;
}

struct DocgenFunctions *docgen_extract_functions(struct LibmatchCursor *cursor,
                                                  const char *comment_start,
                                                  const char *comment_end) {
    struct DocgenFunctions *functions = NULL;
    struct LibmatchCursor new_cursor = libmatch_cursor_init(cursor->buffer,
                                                            cursor->length);
    
    functions = carray_init(functions, FUNCTION);

    /* Find comments */
    while(new_cursor.cursor != new_cursor.length) {
        struct DocgenFunction new_function;

        if(libmatch_string_expect(&new_cursor, comment_start) == 0)
            continue;

        if(docgen_comment_is_type(&new_cursor, comment_start, comment_end, "function") == 0)
            continue;

        /* Parse the comment and append it */
        new_function = docgen_parse_function_comment(&new_cursor, comment_start,
                                                     comment_end);

        carray_append(functions, new_function, FUNCTION);
    }

    return functions;
}

void docgen_extract_functions_free(struct DocgenFunctions *functions) {
    int index = 0;

    for(index = 0; index < carray_length(functions); index++) {
        struct DocgenFunction function = functions->contents[index];

        free(function.errors->contents);
        free(function.errors);

        free(function.parameters->contents);
        free(function.parameters);

        free(function.references->contents);
        free(function.references);

        free(function.inclusions->contents);
        free(function.inclusions);
    }

    free(functions->contents);
    free(functions);
}
