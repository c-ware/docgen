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
 * This file is essentially just a bunch of common functions for lots of
 * the generators. This includes stuff like embeds, for example.
*/

#include "../docgen.h"

#include "../extractors/macros/macros.h"
#include "../extractors/functions/functions.h"
#include "../extractors/structures/structures.h"
#include "../extractors/macro_functions/macro_functions.h"

#include "generators.h"

/*
 * @docgen: function
 * @brief: add spaces to a cstring
 * @name add_offset
 *
 * @description
 * @This function will add an OFFSET number of spaces to a CString. Useful
 * @for lining text up.
 * @description
 *
 * @error: string is NULL
 * @error: offset is negative
 *
 * @param string: the string to add to
 * @type: struct CString
 *
 * @param offset: the number of spaces to add
 * @type: int
*/
static void add_offset(struct CString *string, int offset) {
    int index = 0;

    liberror_is_null(add_offset, string);
    liberror_is_negative(add_offset, offset);

    /* Add a bunch of spaces :) */
    for(index = 0; index < offset; index++)
        cstring_concats(string, " ");
}

/*
 * @docgen: function
 * @brief: format a type with a space if there is no pointer
 * @name: format_pointer
 *
 * @description
 * @Docgen uses the x *y pointer style rather than the x* y style, so
 * @variables without a pointer require a space to separate the type and
 * @the identifier.
 * @description
 *
 * @error: string is NULL
 * @error: type is NULL
 *
 * @param string: the cstring to write the type to
 * @type: struct CString *
 *
 * @param type: the string containing the type
 * @type: const char *
*/
static void format_pointer(struct CString *string, const char *type) {
    liberror_is_null(format_pointer, string);
    liberror_is_null(format_pointer, type);

    cstring_concats(string, type);

    /* Formatting is pretty much already done if there is a pointer,
     * as docgen does x *y, rather than x* y pointer style */
    if(strchr(type, '*') != NULL)
        return;

    cstring_concats(string, " ");
}

/*
 * @docgen: function
 * @brief: continue recursively expanding nested structures
 * @name: make_embedded_structures_recursive
 *
 * @description
 * @This function is used by the make_embedded_structures(cware) function as
 * @the recursive component of generating nested structures.
 * @description
 *
 * @error: structures is NULL
 * @error: string is NULL
 * @error: depth is negative
 * @error: longest_line is negative
 *
 * @param allow_briefs: whether or not briefs are allowed
 * @type: int
 *
 * @param depth: how deep we are in recursion
 * @type: int
 *
 * @param longest_line: the longest line in the structure-- used for alignment
 * @type: int
 *
 * @param structures: the structures to iterate through
 * @type: struct DocgenStructures
 *
 * @param string: the string to write into
 * @type: struct CString
*/
static void make_embedded_structures_recursive(int allow_briefs, int depth, int longest_line,
                                               struct DocgenStructures *structures,
                                               struct CString *string) {
    int index = 0; 
    int field_index = 0;

    for(index = 0; index < carray_length(structures); index++) {
        struct DocgenStructure structure = structures->contents[index];

        if(allow_briefs == 1) {
            add_offset(string, (DOCGEN_INDENTATION * (depth - 1)));
            cstring_concats(string, "/* ");
            cstring_concats(string, structure.brief);
            cstring_concats(string, " /*\n");
        }

        add_offset(string, (DOCGEN_INDENTATION * (depth - 1)));
        cstring_concats(string, "struct {\n");

        /* Iterate over each field and write to it. For each part of the
         * field that is written, extend the field offset which represents
         * how much of the space of the longest line has been taken up. */
        for(field_index = 0; field_index < carray_length(structure.fields); field_index++) {
            int field_offset = 0;
            struct DocgenStructureField field = structure.fields->contents[field_index];

            /* Add a new line to separate the above fields and this structure
             * if there ARE fields, and this is the first nested structure */

            /* Add the offset for the field, and write the field. Docgen
             * favors x *y rather than x* y, so if there is no pointer in
             * the type of the field, add an extra space first */
            add_offset(string, (DOCGEN_INDENTATION * depth));
            cstring_concats(string, field.type);

            if(strchr(field.type, '*') == NULL) {
                cstring_concats(string, " ");

                field_offset++;
            }

            cstring_concats(string, field.name);
            cstring_concats(string, ";");

            /* Add remaining offset then add the description */
            field_offset += strlen(field.type);
            field_offset += strlen(field.name);
            field_offset += strlen("; ");
            field_offset += (DOCGEN_INDENTATION * depth);

            add_offset(string, (longest_line - field_offset));

            cstring_concats(string, "/* ");
            cstring_concats(string, field.description);
            cstring_concats(string, " */\n");
        }

        /* Separate fields and nested structures with a new line (if there is
         * any nested structures and fields */
        if(allow_briefs == 1 && carray_length(structure.fields) > 0 && carray_length(structure.nested) > 0)
            cstring_concats(string, "\n");

        /* Added nested structure declarations */
        if(carray_length(structure.nested) > 0)
            make_embedded_structures_recursive(allow_briefs, depth + 1, longest_line, structure.nested, string);


        add_offset(string, (DOCGEN_INDENTATION * (depth - 1)));
        cstring_concats(string, "} ");
        cstring_concats(string, structure.name);
        cstring_concats(string, ";\n");

        /* Add an extra new line for the next nested structure in the parent
         * structure if we are not the last one */
        if((carray_length(structures) > 0) && (index < (carray_length(structures) - 1)))
            cstring_concats(string, "\n");
    }
}

/*
 * @docgen: function
 * @brief: handles an unrecognized index by producing an error
 * @name: handle_unrecognized_embed
 *
 * @description
 * @This function will report an error for when carray_find(cware) fails due to
 * @the embed request not matching any of the tokens found in the source file.
 * @description
 *
 * @param name: the name of embed request
 * @type: const char *
 *
 * @param target: the token type
 * @type: const char *
 *
 * @param position: the index of the token
 * @type: int
*/
void handle_unrecognized_embed(const char *name, const char *target, int position) {
    if(position >= 0)
        return;

    fprintf(stderr, "docgen: failed to find '%s' named '%s' to embed\n", target, name);
    exit(EXIT_FAILURE);
}







#define HANDLE_UNRECOGNIZED_EMBED(index, type, name)                                      \
do {                                                                                      \
    if(index != -1)                                                                       \
        break;                                                                            \
                                                                                          \
    fprintf(stderr, "docgen: failed to recognize '%s' embed '%s' (%s:%i)\n", #type, name, \
            __FILE__, __LINE__);                                                          \
    exit(EXIT_FAILURE);                                                                   \
} while(0)

struct CStrings *make_embedded_macros(int allow_briefs, struct DocgenMacros macros,
                                      struct Embeds embeds) {
    int index = 0;
    struct CStrings *macro_buffer = carray_init(macro_buffer, CSTRING);

    /* For each embedded macro (constant) make a new CString to represent it. */
    for(index = 0; index < carray_length(&embeds); index++) {
        int macro_index = 0;
        struct CString new_macro_string;
        struct DocgenMacro target_macro;
        struct Embed requested_embed = embeds.contents[index];

        /* This is not an embedded macro we are looking at */
        if(requested_embed.type != DOCGEN_EMBED_CONSTANT)
            continue;

        /* Get the requested macro's data and continue initialization.
         * Search is performed by comparing each macro in the array of
         * macros passed to this function against the embed we are looking at. */
        INIT_VARIABLE(target_macro);
        INIT_VARIABLE(new_macro_string);

        /* Attempt to find the embed's data, but make sure to handle the situation
         * where it is NOT found (when the index is -1) */
        macro_index = carray_find(&macros, requested_embed.name, macro_index, MACRO);
        HANDLE_UNRECOGNIZED_EMBED(macro_index, macro, requested_embed.name);
        target_macro = macros.contents[macro_index];
        new_macro_string = cstring_init("");

        handle_unrecognized_embed(requested_embed.name, "macro", macro_index);
 
        if(allow_briefs == 1) {
            cstring_concats(&new_macro_string, "/* ");
            cstring_concats(&new_macro_string, target_macro.brief);
            cstring_concats(&new_macro_string, " */\n");
        }

        if(target_macro.ifndef) {
            cstring_concats(&new_macro_string, "#ifndef ");
            cstring_concats(&new_macro_string, target_macro.name);
            cstring_concats(&new_macro_string, "\n");
        }

        cstring_concats(&new_macro_string, "#define ");
        cstring_concats(&new_macro_string, target_macro.name);
        cstring_concats(&new_macro_string, " ");
        cstring_concats(&new_macro_string, target_macro.value);
        cstring_concats(&new_macro_string, "\n");

        if(target_macro.ifndef) {
            cstring_concats(&new_macro_string, "#endif\n");
        }

        carray_append(macro_buffer, new_macro_string, CSTRING);
    }

    return macro_buffer;
}

struct CStrings *make_embedded_structures(int allow_briefs,
                                          struct DocgenStructures structures,
                                          struct Embeds embeds) {
    int index = 0;
    struct CStrings *structure_buffer = carray_init(structure_buffer, CSTRING);

    for(index = 0; index < carray_length(&embeds); index++) {
        int field_index = 0;
        int structure_index = 0;
        int longest_field = 0;
        struct CString new_structure_string;
        struct DocgenStructure target_structure;
        struct Embed embed = embeds.contents[index];

        if(embed.type != DOCGEN_EMBED_STRUCTURE)
            continue;

        /* Get the requested structure's data and continue initialization.
         * Search is performed by comparing each macro in the array of
         * macros passed to this function against the embed we are looking at. */
        INIT_VARIABLE(target_structure);
        INIT_VARIABLE(new_structure_string);

        /* Attempt to find the embed's data, but make sure to handle the situation
         * where it is NOT found (when the index is -1) */
        longest_field = docgen_get_longest_field(&structures, 0);
        structure_index = carray_find(&structures, embed.name, structure_index, STRUCTURE);

        HANDLE_UNRECOGNIZED_EMBED(structure_index, structure, embed.name);

        target_structure = structures.contents[structure_index];
        new_structure_string = cstring_init("");

        /* FWIW, we increment the "longest field" by this amount because
         * it just makes the final string look cleaner. */
        longest_field += 4;

        handle_unrecognized_embed(embed.name, "structure", structure_index);

        if(allow_briefs == 1) {
            cstring_concats(&new_structure_string, "/* ");
            cstring_concats(&new_structure_string, target_structure.brief);
            cstring_concats(&new_structure_string, " */\n");
        }

        cstring_concats(&new_structure_string, "struct ");
        cstring_concats(&new_structure_string, target_structure.name);
        cstring_concats(&new_structure_string, " {\n");

        /* Iterate over each field and write to it. For each part of the
         * field that is written, extend the field offset which represents
         * how much of the space of the longest line has been taken up. */
        for(field_index = 0; field_index < carray_length(target_structure.fields); field_index++) {
            int field_offset = 0;
            struct DocgenStructureField field = target_structure.fields->contents[field_index];

            /* Add the offset for the field, and write the field. Docgen
             * favors x *y rather than x* y, so if there is no pointer in
             * the type of the field, add an extra space first */
            add_offset(&new_structure_string, DOCGEN_INDENTATION);
            cstring_concats(&new_structure_string, field.type);

            if(strchr(field.type, '*') == NULL) {
                cstring_concats(&new_structure_string, " ");

                field_offset++;
            }

            cstring_concats(&new_structure_string, field.name);
            cstring_concats(&new_structure_string, ";");

            /* Add remaining offset then add the description */
            field_offset += strlen(field.type);
            field_offset += strlen(field.name);
            field_offset += strlen("; ");
            field_offset += DOCGEN_INDENTATION;

            add_offset(&new_structure_string, (longest_field - field_offset));

            cstring_concats(&new_structure_string, "/* ");
            cstring_concats(&new_structure_string, field.description);
            cstring_concats(&new_structure_string, " */\n");
        }

        /* Added nested structure declarations */
        if(carray_length(target_structure.nested) > 0) {
            if(carray_length(target_structure.fields) > 0)
                cstring_concats(&new_structure_string, "\n");

            make_embedded_structures_recursive(allow_briefs, 2, longest_field, target_structure.nested, &new_structure_string);
        }

        cstring_concats(&new_structure_string, "};\n");
        carray_append(structure_buffer, new_structure_string, STRUCTURE);
    }

    return structure_buffer;
}

struct CStrings *make_embedded_macro_functions(int allow_briefs,
                                               struct DocgenMacroFunctions macro_functions,
                                               struct Embeds embeds) {
    int index = 0;
    int iter_index = 0;
    struct CStrings *macro_function_buffer = carray_init(macro_function_buffer, CSTRING);

    /* For each embedded macro (constant) make a new CString to represent it. */
    for(index = 0; index < carray_length(&embeds); index++) {
        int macro_function_index = 0;
        struct CString new_macro_function_string;
        struct DocgenMacroFunction target_macro_function;
        struct Embed requested_embed = embeds.contents[index];

        /* This is not an embedded macro_function we are looking at */
        if(requested_embed.type != DOCGEN_EMBED_MACRO_FUNCTION)
            continue;

        /* Get the requested macro_function's data and continue initialization.
         * Search is performed by comparing each macro_function in the array of
         * macro_functions passed to this function against the embed we are looking at. */
        INIT_VARIABLE(target_macro_function);
        INIT_VARIABLE(new_macro_function_string);

        /* Attempt to find the embed's data, but make sure to handle the situation
         * where it is NOT found (when the index is -1) */
        macro_function_index = carray_find(&macro_functions, requested_embed.name, macro_function_index, MACRO);

        HANDLE_UNRECOGNIZED_EMBED(macro_function_index, macro_function, requested_embed.name);

        target_macro_function = macro_functions.contents[macro_function_index];
        new_macro_function_string = cstring_init("");

        handle_unrecognized_embed(requested_embed.name, "macro_function", macro_function_index);
 
        if(allow_briefs == 1) {
            cstring_concats(&new_macro_function_string, "/* ");
            cstring_concats(&new_macro_function_string, target_macro_function.brief);
            cstring_concats(&new_macro_function_string, " */\n");
        }

        cstring_concats(&new_macro_function_string, "#define ");
        cstring_concats(&new_macro_function_string, target_macro_function.name);
        cstring_concats(&new_macro_function_string, "(");

        /* Dump the parameters */
        for(iter_index = 0; iter_index < carray_length(target_macro_function.parameters); iter_index++) {
            struct Parameter parameter = target_macro_function.parameters->contents[iter_index];

            cstring_concats(&new_macro_function_string, parameter.name);

            /* Only add a comma if we are the last element in the list */
            if(iter_index < (carray_length(target_macro_function.parameters) - 1))
                cstring_concats(&new_macro_function_string, ", ");
        }

        cstring_concats(&new_macro_function_string, ");\n");
        carray_append(macro_function_buffer, new_macro_function_string, CSTRING);
    }

    return macro_function_buffer;
}

struct CStrings *make_embedded_functions(int allow_briefs,
                                         struct DocgenFunctions functions,
                                         struct Embeds embeds) {
    int index = 0;
    int iter_index = 0;
    struct CStrings *function_buffer = carray_init(function_buffer, CSTRING);

    for(index = 0; index < carray_length(&embeds); index++) {
        int function_index = 0;
        struct CString new_function_string;
        struct DocgenFunction target_function;
        struct Embed requested_embed = embeds.contents[index];

        /* This is not an embedded function we are looking at */
        if(requested_embed.type != DOCGEN_EMBED_FUNCTION)
            continue;

        /* Get the requested function's data and continue initialization.
         * Search is performed by comparing each function in the array of
         * functions passed to this function against the embed we are looking at. */
        INIT_VARIABLE(target_function);
        INIT_VARIABLE(new_function_string);

        /* Attempt to find the embed's data, but make sure to handle the situation
         * where it is NOT found (when the index is -1) */
        function_index = carray_find(&functions, requested_embed.name, function_index, MACRO);

        HANDLE_UNRECOGNIZED_EMBED(function_index, function, requested_embed.name);

        target_function = functions.contents[function_index];
        new_function_string = cstring_init("");

        handle_unrecognized_embed(requested_embed.name, "function", function_index);

        if(allow_briefs == 1) {
            cstring_concats(&new_function_string, "/* ");
            cstring_concats(&new_function_string, target_function.brief);
            cstring_concats(&new_function_string, " */\n");
        }

        /* Base signature of the function. If the function has no return type, then
         * assume it is void. */
        if(strlen(target_function.return_data.return_type) == 0)
            format_pointer(&new_function_string, "void");
        else
            format_pointer(&new_function_string, target_function.return_data.return_type);

        cstring_concats(&new_function_string, target_function.name);
        cstring_concats(&new_function_string, "(");

        /* No parameters? Use `int x(void)` rather than `int x()` */
        if(carray_length(target_function.parameters) == 0)
        cstring_concats(&new_function_string, "void");

        /* Write the parameters */
        for(iter_index = 0; iter_index < carray_length(target_function.parameters); iter_index++) {
            struct Parameter parameter = target_function.parameters->contents[iter_index];

            format_pointer(&new_function_string, parameter.type);
            cstring_concats(&new_function_string, parameter.name);

            /* Only add a comma if this is NOT the last item */
            if(iter_index == (carray_length(target_function.parameters) - 1)) 
                continue;

            cstring_concats(&new_function_string, ", ");
        }

        cstring_concats(&new_function_string, ");\n");
        carray_append(function_buffer, new_function_string, CSTRING);
    }

    return function_buffer;
}

char *duplicate_string(const char *string) {
    char *new_string = NULL;

    liberror_is_null(duplicate_string, string);

    new_string = malloc(sizeof(char) * (strlen(string) + 1));
    new_string[0] = 0x00;
    strncat(new_string, string, strlen(string));

    return new_string;
}
