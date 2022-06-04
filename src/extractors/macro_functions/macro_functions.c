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
    new_macro_function.errors = carray_init(new_macro_function.errors, MACRO_FUNCTION_ERROR);
    new_macro_function.parameters = carray_init(new_macro_function.parameters, MACRO_FUNCTION_PARAMETER);
    new_macro_function.references = carray_init(new_macro_function.references, REFERENCE);
    new_macro_function.inclusions = carray_init(new_macro_function.inclusions, INCLUDE);

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
            docgen_extract_field_line("name", DOCGEN_MACRO_FUNCTION_NAME_LENGTH, cursor->line,
                                      new_tag.line, new_macro_function.name);

        else if(strcmp(tag_name.name, "brief") == 0)
            docgen_extract_field_line("brief", DOCGEN_MACRO_FUNCTION_BRIEF_LENGTH, cursor->line,
                                      new_tag.line, new_macro_function.brief);

        else if(strcmp(tag_name.name, "description") == 0)
            docgen_extract_field_block("description", DOCGEN_MACRO_FUNCTION_DESCRIPTION_LENGTH, cursor,
                                      new_tag.line, new_macro_function.description);

        else if(strcmp(tag_name.name, "notes") == 0)
            docgen_extract_field_block("notes", DOCGEN_MACRO_FUNCTION_NOTES_LENGTH, cursor,
                                      new_tag.line, new_macro_function.notes);

        else if(strcmp(tag_name.name, "example") == 0)
            docgen_extract_field_block("example", DOCGEN_MACRO_FUNCTION_EXAMPLE_LENGTH, cursor,
                                      new_tag.line, new_macro_function.example);

        else if(strcmp(tag_name.name, "include") == 0) {
            struct Inclusion new_inclusion;

            memset(&new_inclusion, 0, sizeof(struct Inclusion));
            new_inclusion.type = DOCGEN_INCLUSION_LOCAL;

            docgen_extract_field_line("include", DOCGEN_MACRO_FUNCTION_INCLUSION_LENGTH,
                                      cursor->line, new_tag.line, new_inclusion.path);

            carray_append(new_macro_function.inclusions, new_inclusion, INCLUDE);
        }

        else if(strcmp(tag_name.name, "isystem") == 0) {
            struct Inclusion new_inclusion;

            memset(&new_inclusion, 0, sizeof(struct Inclusion));
            new_inclusion.type = DOCGEN_INCLUSION_SYSTEM;

            docgen_extract_field_line("isystem", DOCGEN_MACRO_FUNCTION_INCLUSION_LENGTH,
                                      cursor->line, new_tag.line, new_inclusion.path);

            carray_append(new_macro_function.inclusions, new_inclusion, INCLUDE);
        }

        else if(strcmp(tag_name.name, "error") == 0) {
            struct DocgenMacroFunctionError new_error;

            memset(&new_error, 0, sizeof(struct DocgenMacroFunctionError));
            docgen_extract_field_line("error", DOCGEN_MACRO_FUNCTION_ERROR_DESCRIPTION_LENGTH,
                                      cursor->line, new_tag.line, new_error.description);

            carray_append(new_macro_function.errors, new_error, MACRO_FUNCTION_ERROR);
        }

        else if(strcmp(tag_name.name, "reference") == 0) {
            struct Reference new_reference;

            memset(&new_reference, 0, sizeof(struct Reference));
            new_reference = docgen_extract_reference(cursor, new_tag);

            carray_append(new_macro_function.references, new_reference, REFERENCE);
        }

        else if(strcmp(tag_name.name, "param") == 0) {
            struct DocgenTag type_tag;
            struct DocgenTagName type_tag_name;
            struct DocgenMacroFunctionParameter new_parameter;

            memset(&type_tag, 0, sizeof(struct DocgenTag));
            memset(&type_tag_name, 0, sizeof(struct DocgenTagName));
            memset(&new_parameter, 0, sizeof(struct DocgenMacroFunctionParameter));

            docgen_extract_field_line_arg("param", new_tag.line, DOCGEN_MACRO_FUNCTION_PARAMETER_NAME_LENGTH,
                                          new_parameter.name, DOCGEN_MACRO_FUNCTION_PARAMETER_DESCRIPTION_LENGTH,
                                          new_parameter.description, cursor->line);

            carray_append(new_macro_function.parameters, new_parameter, MACRO_FUNCTION_PARAMETER);
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
    }

    free(macro_functions->contents);
    free(macro_functions);
}
