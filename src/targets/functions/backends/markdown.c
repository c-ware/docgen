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
 * The backend for converting 'function' tokens into Markdown files
*/

#include <string.h>

#include "../functions.h"

#define write_inclusions(array)                                            \
do {                                                                       \
    int __INCLUSION_INDEX = 0;                                             \
                                                                           \
    while(__INCLUSION_INDEX < carray_length(array)) {                      \
        struct Inclusion inclusion = (array)->contents[__INCLUSION_INDEX]; \
                                                                           \
        if(inclusion.type == DOCGEN_INCLUSION_LOCAL) {                     \
            fprintf(location, "#include \"%s\"", inclusion.path);          \
        } else if(inclusion.type == DOCGEN_INCLUSION_SYSTEM) {             \
            fprintf(location, "#include <%s>", inclusion.path);            \
        }                                                                  \
                                                                           \
        fprintf(location, "%c", '\n');                                     \
                                                                           \
        __INCLUSION_INDEX++;                                               \
    }                                                                      \
} while(0)

static int number_of_embedded_types(struct Embeds *embeds, int type) {
    int index = 0;
    int counted = 0;

    liberror_is_null(number_of_embedded_types, embeds);

    for(index = 0; index < carray_length(embeds); index++) {
        if(embeds->contents[index].type != type)
            continue;

        counted++;

    }

    return counted;
}

/* 
 * EMBEDDING LOGIC
*/
static void embed_constants(struct DocgenFunction function,
                            struct DocgenMacros *macros,
                            FILE *location) {
    int index = 0;
    int embeds = 0;
    int number_of_embeds = number_of_embedded_types(function.embeds, DOCGEN_EMBED_CONSTANT);
    int unadded_embeds = number_of_embedded_types(function.embeds, DOCGEN_EMBED_MACRO_FUNCTION) +
                         number_of_embedded_types(function.embeds, DOCGEN_EMBED_STRUCTURE) +
                         number_of_embedded_types(function.embeds, DOCGEN_EMBED_FUNCTION);

    if(number_of_embeds != 0)
        fprintf(location, "%c", '\n');

    /* Constant Embeds */
    for(index = 0; index < carray_length(function.embeds); index++) {
        int macro_index = 0;
        struct DocgenMacro macro;
        struct Embed embed = function.embeds->contents[index];

        if(embed.type != DOCGEN_EMBED_CONSTANT)
            continue;

        embeds++;

        macro_index = carray_find(macros, embed.name, macro_index, MACRO);
        macro = macros->contents[macro_index];

        /* Write the comment if there is one */
        if(strlen(macro.brief) != 0)
            fprintf(location, "/* %s */\n", macro.brief);
        
        /* Wrap the #define in an #ifndef */
        if(macro.ifndef == 1)
            fprintf(location, "#ifndef %s\n", macro.name);

        fprintf(location, "#define %s %s\n", macro.name, macro.value);

        if(macro.ifndef == 1)
            fprintf(location, "%s", "#endif\n");

        /* Add an extra new line */
        if(embeds < number_of_embeds)
            fprintf(location, "%c", '\n');
    }

    /* Do not add an extra new line unless there is a constant, and
     * there are other embeds that need to be added. That way, we do
     * not add an extra unnecessary line. */
    if(number_of_embeds != 0 && unadded_embeds != 0)
        fprintf(location, "%c", '\n');
}

static void embed_macro_functions(struct DocgenFunction function,
                            struct DocgenMacroFunctions *macro_functions,
                            FILE *location) {
    int index = 0;
    int embed_counter = 0;
    int number_of_embeds = number_of_embedded_types(function.embeds, DOCGEN_EMBED_MACRO_FUNCTION);
    int unadded_embeds = number_of_embedded_types(function.embeds, DOCGEN_EMBED_STRUCTURE) +
                         number_of_embedded_types(function.embeds, DOCGEN_EMBED_FUNCTION);

    /* Macro Function Embeds */
    for(index = 0; index < carray_length(function.embeds); index++) {
        int parameter_index = 0;
        int macro_function_index = 0;
        struct DocgenMacroFunction macro_function;
        struct Embed embed = function.embeds->contents[index];

        if(embed.type != DOCGEN_EMBED_MACRO_FUNCTION)
            continue;

        macro_function_index = carray_find(macro_functions, embed.name, macro_function_index, MACRO_FUNCTION);
        embed_counter++;

        if(macro_function_index == -1) {
            fprintf(stderr, "docgen: attempt to embed unknown macro_function '%s'\n", embed.name);
            exit(EXIT_FAILURE);
        }

        macro_function = macro_functions->contents[macro_function_index];

        /* Write the comment if there is one, and its allowed */
        if((strlen(macro_function.brief) != 0) && function.macro_function_briefs == 1)
            fprintf(location, "/* %s */\n", macro_function.brief);

        /* "#define NAME" */
        fprintf(location, "#define %s(", macro_function.name);

        /* Generate the arguments and parameters */
        for(parameter_index = 0; parameter_index < carray_length(macro_function.parameters); parameter_index++) {
            struct DocgenMacroFunctionParameter parameter;

            parameter = macro_function.parameters->contents[parameter_index];

            /* Decide whether or not to display the trailing comma */
            if(parameter_index == carray_length(macro_function.parameters) - 1) {
                fprintf(location, "%s", parameter.name);
            } else {
                fprintf(location, "%s, ", parameter.name);
            }
        }

        fprintf(location, "%s", ");\n");

        /* If we are the last macro function, there is no need to add an extra
         * new line. We only add an extra new line if briefs are allowed. Otherwise,
         * it looks weird. */
        if(embed_counter != number_of_embeds && function.macro_function_briefs == 1)
            fprintf(location, "%s", "\n");
    }

    /* Do not add an extra new line unless there is a constant, and
     * there are other embeds that need to be added. That way, we do
     * not add an extra unnecessary line. */
    if(number_of_embeds != 0 && unadded_embeds != 0)
        fprintf(location, "%c", '\n');
}

static void embed_structures_recurse(struct DocgenFunction function,
                                     struct DocgenStructures *structures,
                                     FILE *location, int longest, int depth) {
    int index = 0;

    for(index = 0; index < carray_length(structures); index++) {
        int pad_index = 0;
        int field_index = 0;
        struct DocgenStructure structure = structures->contents[index];

        /* Add manual padding */
        for(pad_index = 0; pad_index < depth * 4; pad_index++) {
            fprintf(location, "%c", ' ');
        } 

        fprintf(location, "%s", "struct {\n");

        /* Display all fields */
        for(field_index = 0; field_index < carray_length(structure.fields); field_index++) {
            struct DocgenStructureField field;

            if(structure.fields == NULL)
                break;

            field = structure.fields->contents[field_index];

            /* Add manual padding */
            for(pad_index = 0; pad_index < depth * 4; pad_index++) {
                fprintf(location, "%c", ' ');
            } 

            /* Should be a space if there is no pointer, so regular
             * fields will not have an asterisk. Lack of pointer should
             * also have a shorter length since the length calculation
             * does not account for the artifical insertion of a space
             * between field and type names in situations without pointers. */
            if(strchr(field.type, '*') == NULL) {
                fprintf(location, "    %s %s; ", field.type, field.name);
                docgen_do_padding(field, longest - 1, depth, location);
            } else {
                fprintf(location, "    %s%s; ", field.type, field.name);
                docgen_do_padding(field, longest, depth, location);
            }


            if(strlen(field.description) != 0)
                fprintf(location, "/* %s */\n", field.description);
        }

        /* Display nested structures */
        embed_structures_recurse(function, structure.nested, location, longest,
                                 depth + 1);

        /* Add manual padding */
        for(pad_index = 0; pad_index < depth * 4; pad_index++) {
            fprintf(location, "%c", ' ');
        } 

        fprintf(location, "} %s;\n", structure.name);
    }
}

static void embed_structures(struct DocgenFunction function,
                             struct DocgenStructures *structures,
                             FILE *location) {
    int index = 0;
    int embeds = 0;
    int longest_field = docgen_get_longest_field(structures, 0);
    int number_of_embeds = number_of_embedded_types(function.embeds, DOCGEN_EMBED_CONSTANT);
    int unadded_embeds = number_of_embedded_types(function.embeds, DOCGEN_EMBED_FUNCTION);

    for(index = 0; index < carray_length(function.embeds); index++) {
        int field_index = 0;
        int structure_index = 0;
        struct DocgenStructure structure;
        struct Embed embed = function.embeds->contents[index];

        if(embed.type != DOCGEN_EMBED_STRUCTURE)
            continue;

        embeds++;

        structure_index = carray_find(structures, embed.name, structure_index, STRUCTURE);
        liberror_is_number(docgen_function_manpage_embed_structures, structure_index, "%i", -1);
        structure = structures->contents[structure_index];

        if(strlen(structure.brief) != 0)
            fprintf(location, "/* %s */\n", structure.brief);

        fprintf(location, "struct %s {\n", structure.name);

        /* Display all fields */
        for(field_index = 0; field_index < carray_length(structure.fields); field_index++) {
            struct DocgenStructureField field;

            field = structure.fields->contents[field_index];

            /* Should be a space if there is no pointer, so regular
             * fields will not have an asterisk. Lack of pointer should
             * also have a shorter length since the length calculation
             * does not account for the artifical insertion of a space
             * between field and type names in situations without pointers. */
            if(strchr(field.type, '*') == NULL) {
                fprintf(location, "    %s %s; ", field.type, field.name);
                docgen_do_padding(field, longest_field - 1, 0, location);
            } else {
                fprintf(location, "    %s%s; ", field.type, field.name);
                docgen_do_padding(field, longest_field, 0, location);
            }

            if(strlen(field.description) != 0)
                fprintf(location, "/* %s */\n", field.description);
        }

        embed_structures_recurse(function, structure.nested, location, longest_field, 1);

        fprintf(location, "%s", "};\n");

        /* Add an extra new line */
        if(embeds < number_of_embeds)
            fprintf(location, "%c", '\n');
    }

    /* Do not add an extra new line unless there is a constant, and
     * there are other embeds that need to be added. That way, we do
     * not add an extra unnecessary line. */
    if(number_of_embeds != 0 && unadded_embeds != 0)
        fprintf(location, "%c", '\n');
}

static void embed_functions(struct DocgenFunction function,
                            struct DocgenFunctions *functions,
                            FILE *location) {
    int index = 0;
    int embed_counter = 0;
    int number_of_embeds = number_of_embedded_types(function.embeds, DOCGEN_EMBED_FUNCTION);

    /* Function Embeds */
    for(index = 0; index < carray_length(function.embeds); index++) {
        int function_index = 0;
        int parameter_index = 0;
        struct DocgenFunction embedded_function_data;
        struct Embed embed = function.embeds->contents[index];

        if(embed.type != DOCGEN_EMBED_FUNCTION)
            continue;

        embed_counter++;
        function_index = carray_find(functions, embed.name, function_index, FUNCTION);

        if(function_index == -1) {
            fprintf(stderr, "docgen: attempt to embed unknown embedded_function_data '%s'\n", embed.name);
            exit(EXIT_FAILURE);
        }

        embedded_function_data = functions->contents[function_index];

        /* Write the comment if there is one, and its allowed */
        if((strlen(embedded_function_data.brief) != 0) && function.function_briefs == 1)
            fprintf(location, "/* %s */\n", embedded_function_data.brief);

        /* "void" return type */
        if(embedded_function_data.return_data.return_type[0] == '\0')
            strcat(embedded_function_data.return_data.return_type, "void ");

        /* Function signature
         * Non-pointer types should not require a space after them to produce
         * the space, but asterisk attached to type instead of name should.
        */
        if(strchr(embedded_function_data.return_data.return_type, '*') == NULL)
            fprintf(location, "%s %s(", embedded_function_data.return_data.return_type, embedded_function_data.name);
        else
            fprintf(location, "%s%s(", embedded_function_data.return_data.return_type, embedded_function_data.name);

        /* Generate the arguments and parameters */
        for(parameter_index = 0; parameter_index < carray_length(embedded_function_data.parameters); parameter_index++) {
            int is_ptr = 0;
            struct DocgenFunctionParameter parameter;

            parameter = embedded_function_data.parameters->contents[parameter_index];

            if(strchr(parameter.type, '*') != NULL)
                is_ptr = 1;

            /* Decide whether or not to display the trailing comma */
            if(parameter_index == carray_length(embedded_function_data.parameters) - 1) {
                if(is_ptr == 0) {
                    fprintf(location, "%s %s", parameter.type, parameter.name);
                } else {
                    fprintf(location, "%s%s", parameter.type, parameter.name);
                }
            } else {
                if(is_ptr == 0) {
                    fprintf(location, "%s %s, ", parameter.type, parameter.name);
                } else {
                    fprintf(location, "%s%s, ", parameter.type, parameter.name);
                }
            }
        }

        /* If theres no parameters, simply do void */
        if(carray_length(embedded_function_data.parameters) == 0)
            fprintf(location, "%s", "void");
            
        fprintf(location, "%s", ");\n");

        /* If there was a function brief displayed, we want to add a new line
         * so that the next one's function brief has a new line before it.
         * That is, unless we are the last function. */
        if(embed_counter == number_of_embeds) {
            /* If, however, we are the last one, and function briefs ARE
             * being displayed, it looks better to have the function signature
             * of the manual page's target function to have an extra new line
             * between it and the embedded functions. */
            if((strlen(embedded_function_data.brief) != 0) && function.function_briefs == 1)
                fprintf(location, "%s", "\n");


            continue;
        }

        if((strlen(embedded_function_data.brief) != 0) && function.function_briefs == 1)
            fprintf(location, "%c", '\n');
    }
}


static void head(FILE *location, struct DocgenArguments arguments, struct DocgenFunction function) {
    fprintf(location, "# %s.md\n\n", function.name);
    fprintf(location, "%s", "### NAME\n");
    fprintf(location, "%s - %s\n\n", function.name, function.brief);
}

static void synopsis(FILE *location, struct DocgenArguments arguments,
                     struct DocgenFunction function, struct LibmatchCursor cursor) {
    int index = 0;
    struct DocgenMacros *macros = NULL;
    struct DocgenFunctions *functions = NULL;
    struct DocgenStructures *structures = NULL;
    struct DocgenMacroFunctions *macro_functions = NULL;

    /* Comment junk */
    const char *comment_start = docgen_get_comment_start(arguments);
    const char *comment_end = docgen_get_comment_end(arguments);

    /* The docgen_extract_XXXXX functions will take a cursor and generate a
     * copy of the one it is given, so technically the & is pointless,
     * but its just more refactoring work to do. */
    macros = docgen_extract_macros(&cursor, comment_start, comment_end);
    functions = docgen_extract_functions(&cursor, comment_start, comment_end);
    structures = docgen_extract_structures(&cursor, comment_start, comment_end); 
    macro_functions = docgen_extract_macro_functions(&cursor, comment_start, comment_end); 

    fprintf(location, "%s", "### SYNOPSIS\n");
    fprintf(location, "%s", "```c\n");

    write_inclusions(arguments.inclusions);
    write_inclusions(function.inclusions);

    if(carray_length(arguments.inclusions) != 0 || carray_length(function.inclusions) == 0)
        fprintf(location, "%c", '\n');

    embed_constants(function, macros, location);
    embed_macro_functions(function, macro_functions, location);
    embed_structures(function, structures, location);
    embed_functions(function, functions, location);

    /* If no things were embedded, do not add an extra new line. The new line
     * added after the inclusion will suffice. If there were things embedded,
     * we only want to add an extra line based off whether or not macro functions
     * were embedded */
    if(carray_length(function.embeds) != 0)
        fprintf(location, "%c", '\n');

    /* Write the function signature. Since this is generating documentation
     * for funcions, having the manual's function be aligned with the
     * embedded functions would be nice. (This is why we have functions
     * generated last) */
    fprintf(location, "%s %s(", function.return_data.return_type,  function.name);

    /* Function parameters */
    for(index = 0; index < carray_length(function.parameters); index++) {
        struct DocgenFunctionParameter parameter = function.parameters->contents[index];

        /* Use backticks for generating a monospaced font for code. */
        if(strchr(parameter.type, '*') == NULL)
            fprintf(location, "%s %s", parameter.type, parameter.name);
        else
            fprintf(location, "%s%s", parameter.type, parameter.name);

        /* Do not output a comma */
        if(index == (carray_length(function.parameters) - 1))
            continue;
        
        fprintf(location, "%s", ", ");
    }

    fprintf(location, "%s", ")\n");
    fprintf(location, "%s", "```\n\n");

    docgen_extract_macros_free(macros);
    docgen_extract_functions_free(functions);
    docgen_extract_structures_free(structures);
    docgen_extract_macro_functions_free(macro_functions);
}

static void description(FILE *location, struct DocgenArguments arguments, struct DocgenFunction function) {
    fprintf(location, "%s", "### DESCRIPTION\n");
    fprintf(location, "%s", function.description);
    fprintf(location, "%s", "\n");
}

static void return_value(FILE *location, struct DocgenArguments arguments, struct DocgenFunction function) {
    fprintf(location, "%s", "### RETURN VALUE\n");

    /* No return value, or no description provided */
    if(function.return_data.return_value[0] == '\0')
        fprintf(location, "%s", "This function has no return value.\n");
    else
        fprintf(location, "This function will return %s\n", function.return_data.return_value);

    fprintf(location, "%s", "\n");
}

static void notes(FILE *location, struct DocgenArguments arguments, struct DocgenFunction function) {
    fprintf(location, "%s", "### NOTES\n");

    /* No notes */
    if(function.notes[0] == '\0')
        fprintf(location, "%s", "This function has no notes.\n");
    else
        fprintf(location, "%s\n", function.notes);
}

static void examples(FILE *location, struct DocgenArguments arguments, struct DocgenFunction function) {
    fprintf(location, "%s", "### EXAMPLES\n");

    /* No examples */
    if(function.example[0] == '\0')
        fprintf(location, "%s", "This function has no examples.\n");
    else
        fprintf(location, "%s\n", function.example);
}

static void see_also(FILE *location, struct DocgenArguments arguments, struct DocgenFunction function) {
    int index = 0;

    fprintf(location, "%s", "### SEE ALSO\n");

    if(carray_length(function.references) == 0) {
        fprintf(location, "%s", "This function has no references.\n");
        return;
    }

    while(index < carray_length(function.references)) {
        struct Reference reference = function.references->contents[index];

        fprintf(location, "%s(%s)", reference.manual, reference.section);

        /* Add a comma */
        if(index < (carray_length(function.references) - 1))
            fprintf(location, "%s", ", ");

        index++;
    }

    fprintf(location, "%c", '\n');
}

void docgen_functions_markdown(struct DocgenArguments arguments, struct LibmatchCursor cursor, struct DocgenFunction function) {
    FILE *location = NULL;
    char file_path[LIBPATH_MAX_PATH + 1];

    /* Open the file */
    docgen_create_file_path(arguments, function.name, file_path, LIBPATH_MAX_PATH);
    libpath_join_path(file_path, LIBPATH_MAX_PATH, "./doc/", function.name,
                      ".md", NULL);
    location = fopen(file_path, "w");

    /* Dump parts of the Markdown */
    head(location, arguments, function);
    synopsis(location, arguments, function, cursor);
    description(location, arguments, function);
    return_value(location, arguments, function);
    notes(location, arguments, function);
    examples(location, arguments, function);
    see_also(location, arguments, function);

    fclose(location);
}
