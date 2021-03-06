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
 * This file is used to extract macro functions from a buffer.
*/

#include "macro_functions.h"

struct DocgenMacroFunction docgen_parse_macro_function_comment(struct LibmatchCursor *cursor,
                                                               const char *comment_start,
                                                               const char *comment_end) {
    struct DocgenMacroFunction new_macro_function;
    
    memset(&new_macro_function, 0, sizeof(struct DocgenMacroFunction));

    /* Initialize function container */
    new_macro_function.errors = carray_init(new_macro_function.errors, ERROR);
    new_macro_function.parameters = carray_init(new_macro_function.parameters, PARAMETER);
    new_macro_function.references = carray_init(new_macro_function.references, REFERENCE);
    new_macro_function.inclusions = carray_init(new_macro_function.inclusions, INCLUDE);
    new_macro_function.embeds = carray_init(new_macro_function.embeds, EMBED);

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
        if(strcmp(tag_name.name, "name") == 0) {
            docgen_extract_field_line("name", new_macro_function.name, DOCGEN_MACRO_FUNCTION_NAME_LENGTH,
                                      cursor->line, new_tag.line);
        } else if(strcmp(tag_name.name, "brief") == 0) {
            docgen_extract_field_line("brief", new_macro_function.brief, DOCGEN_MACRO_FUNCTION_BRIEF_LENGTH,
                                      cursor->line, new_tag.line);
        } else if(strcmp(tag_name.name, "description") == 0) {
            docgen_extract_field_block("description", new_macro_function.description,
                                       DOCGEN_MACRO_FUNCTION_DESCRIPTION_LENGTH, cursor, new_tag.line);
        } else if(strcmp(tag_name.name, "notes") == 0) {
            docgen_extract_field_block("notes", new_macro_function.notes,
                                       DOCGEN_MACRO_FUNCTION_NOTES_LENGTH, cursor, new_tag.line);
        } else if(strcmp(tag_name.name, "example") == 0) {
            docgen_extract_field_block("example", new_macro_function.example,
                                       DOCGEN_MACRO_FUNCTION_EXAMPLE_LENGTH, cursor, new_tag.line);
        } else if(strcmp(tag_name.name, "include") == 0) {
            struct Inclusion new_inclusion;

            memset(&new_inclusion, 0, sizeof(struct Inclusion));
            new_inclusion.type = DOCGEN_INCLUSION_LOCAL;

            docgen_extract_field_line("include", new_inclusion.path, DOCGEN_MACRO_FUNCTION_INCLUSION_LENGTH,
                                      cursor->line, new_tag.line);

            carray_append(new_macro_function.inclusions, new_inclusion, INCLUDE);
        } else if(strcmp(tag_name.name, "isystem") == 0) {
            struct Inclusion new_inclusion;

            memset(&new_inclusion, 0, sizeof(struct Inclusion));
            new_inclusion.type = DOCGEN_INCLUSION_SYSTEM;

            docgen_extract_field_line("isystem", new_inclusion.path, DOCGEN_MACRO_FUNCTION_INCLUSION_LENGTH,
                                      cursor->line, new_tag.line);

            carray_append(new_macro_function.inclusions, new_inclusion, INCLUDE);
        } else if(strcmp(tag_name.name, "error") == 0) {
            struct Error new_error;

            memset(&new_error, 0, sizeof(struct Error));
            docgen_extract_field_line("error", new_error.description, DOCGEN_ERROR_DESCRIPTION_LENGTH,
                                      cursor->line, new_tag.line);

            carray_append(new_macro_function.errors, new_error, ERROR);
        } else if(strcmp(tag_name.name, "reference") == 0) {
            struct Reference new_reference;

            memset(&new_reference, 0, sizeof(struct Reference));
            new_reference = docgen_extract_reference(cursor, new_tag);

            carray_append(new_macro_function.references, new_reference, REFERENCE);
        } else if(strcmp(tag_name.name, "param") == 0) {
            struct DocgenTag type_tag;
            struct DocgenTagName type_tag_name;
            struct Parameter new_parameter;

            memset(&type_tag, 0, sizeof(struct DocgenTag));
            memset(&type_tag_name, 0, sizeof(struct DocgenTagName));
            memset(&new_parameter, 0, sizeof(struct Parameter));

            docgen_extract_field_line_arg("param", new_parameter.name, DOCGEN_MACRO_FUNCTION_PARAMETER_NAME_LENGTH,
                                          new_parameter.description, DOCGEN_MACRO_FUNCTION_PARAMETER_DESCRIPTION_LENGTH,
                                          cursor->line, new_tag.line);

            carray_append(new_macro_function.parameters, new_parameter, PARAMETER);
        } else if(strcmp(tag_name.name, "setting") == 0) {
            char setting_name[DOCGEN_MACRO_FUNCTION_SETTING_LENGTH + 1];

            memset(setting_name, 0, sizeof(setting_name));
            docgen_extract_field_line("setting", setting_name, DOCGEN_MACRO_FUNCTION_SETTING_LENGTH,
                                      cursor->line, new_tag.line);

            if(strcmp(setting_name, "func-briefs") == 0)
                new_macro_function.function_briefs = 1;
            else if(strcmp(setting_name, "mfunc-briefs") == 0)
                new_macro_function.macro_function_briefs = 1;
            else if(strcmp(setting_name, "struct-briefs") == 0)
                new_macro_function.structure_briefs = 1;
            else if(strcmp(setting_name, "macro-briefs") == 0)
                new_macro_function.macro_briefs = 1;
            else {
                fprintf(stderr, "docgen: unknown setting '%s' on line %i\n", setting_name, cursor->line);
                exit(EXIT_FAILURE);
            }
        } else if(strcmp(tag_name.name, "embed") == 0) {
            struct Embed new_embed;
            char embed_type[DOCGEN_EMBED_TYPE_LENGTH + 1];

            memset(embed_type, 0, sizeof(embed_type));
            memset(&new_embed, 0, sizeof(struct Embed));

            docgen_extract_field_line_arg("embed", embed_type, DOCGEN_EMBED_TYPE_LENGTH,
                                   new_embed.name, DOCGEN_EMBED_NAME_LENGTH,
                                   cursor->line, new_tag.line);

            if(strcmp(embed_type, "structure") == 0)
                new_embed.type = DOCGEN_EMBED_STRUCTURE;
            else if(strcmp(embed_type, "function") == 0)
                new_embed.type = DOCGEN_EMBED_FUNCTION;
            else if(strcmp(embed_type, "constant") == 0)
                new_embed.type = DOCGEN_EMBED_CONSTANT;
            else if(strcmp(embed_type, "macro_function") == 0)
                new_embed.type = DOCGEN_EMBED_MACRO_FUNCTION;
            else {
                fprintf(stderr, "docgen: unknown embed type '%s' at line %i\n", embed_type, cursor->line);
                exit(EXIT_FAILURE);
            }

            carray_append(new_macro_function.embeds, new_embed, EMBED);
        } else {
            fprintf(stderr, "docgen: unknown tag '%s' in function extractor on line %i (%s)\n",
                    tag_name.name, cursor->line, new_tag.line);
            exit(EXIT_FAILURE);
        }
    }

break_loop:

    return new_macro_function;
}

struct DocgenMacroFunctions *docgen_extract_macro_functions(struct LibmatchCursor *cursor,
                                                            const char *comment_start,
                                                            const char *comment_end) {
    struct DocgenMacroFunctions *macro_functions = NULL;
    struct LibmatchCursor new_cursor = libmatch_cursor_init(cursor->buffer,
                                                            cursor->length);
    
    macro_functions = carray_init(macro_functions, MACRO_FUNCTION);

    /* Find comments */
    while(new_cursor.cursor != new_cursor.length) {
        struct DocgenMacroFunction new_macro_function;

        if(libmatch_string_expect(&new_cursor, comment_start) == 0)
            continue;

        if(docgen_comment_is_type(&new_cursor, comment_start, comment_end, "macro_function") == 0)
            continue;

        /* Parse the comment and append it */
        new_macro_function = docgen_parse_macro_function_comment(&new_cursor, comment_start,
                                                     comment_end);

        carray_append(macro_functions, new_macro_function, MACRO_FUNCTION);
    }

    return macro_functions;
}

void docgen_extract_macro_functions_free(struct DocgenMacroFunctions *macro_functions) {
    int index = 0;

    for(index = 0; index < carray_length(macro_functions); index++) {
        struct DocgenMacroFunction macro_function = macro_functions->contents[index];

        free(macro_function.errors->contents);
        free(macro_function.errors);

        free(macro_function.parameters->contents);
        free(macro_function.parameters);

        free(macro_function.references->contents);
        free(macro_function.references);

        free(macro_function.inclusions->contents);
        free(macro_function.inclusions);

        free(macro_function.embeds->contents);
        free(macro_function.embeds);
    }

    free(macro_functions->contents);
    free(macro_functions);
}
