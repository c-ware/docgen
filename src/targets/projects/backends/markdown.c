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
 * The backend for converting 'project' tokens into Markdown files
*/

#include <string.h>

#include "../projects.h"

#define write_inclusions(array)                                            \
do {                                                                       \
    int __INCLUSION_INDEX = 0;                                             \
                                                                           \
    while(__INCLUSION_INDEX < carray_length(array)) {                      \
        struct Inclusion inclusion = (array)->contents[__INCLUSION_INDEX]; \
                                                                           \
        if(arguments.md_mono == 1)                                         \
            fprintf(location, "%c", '`');                                  \
                                                                           \
        if(inclusion.type == DOCGEN_INCLUSION_LOCAL) {                     \
            fprintf(location, "#include \"%s\"", inclusion.path);          \
        } else if(inclusion.type == DOCGEN_INCLUSION_SYSTEM) {             \
            fprintf(location, "#include <%s>", inclusion.path);            \
        }                                                                  \
                                                                           \
        if(arguments.md_mono == 1)                                         \
            fprintf(location, "%c", '`');                                  \
                                                                           \
        fprintf(location, "%c", '\n');                                     \
                                                                           \
        __INCLUSION_INDEX++;                                               \
    }                                                                      \
} while(0)

static int number_of_embedded_types(struct DocgenProjectEmbeds *embeds, int type) {
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
static void embed_constants(struct DocgenProject project,
                            struct DocgenMacros *macros,
                            FILE *location) {
    int index = 0;
    int embeds = 0;
    int number_of_embeds = number_of_embedded_types(project.embeds, DOCGEN_PROJECT_EMBED_CONSTANT);

    fprintf(location, "```c\n");

    /* Constant Embeds */
    for(index = 0; index < carray_length(project.embeds); index++) {
        int macro_index = 0;
        struct DocgenMacro macro;
        struct DocgenProjectEmbed embed = project.embeds->contents[index];

        if(embed.type != DOCGEN_PROJECT_EMBED_CONSTANT)
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

    fprintf(location, "```\n\n");
}

static void embed_macro_functions(struct DocgenProject project,
                            struct DocgenMacroFunctions *macro_functions,
                            FILE *location) {
    int index = 0;

    fprintf(location, "```c\n");

    /* Macro Function Embeds */
    for(index = 0; index < carray_length(project.embeds); index++) {
        int parameter_index = 0;
        int macro_function_index = 0;
        struct DocgenMacroFunction macro_function;
        struct DocgenProjectEmbed embed = project.embeds->contents[index];

        if(embed.type != DOCGEN_PROJECT_EMBED_MACRO_FUNCTION)
            continue;

        macro_function_index = carray_find(macro_functions, embed.name, macro_function_index, MACRO_FUNCTION);

        if(macro_function_index == -1) {
            fprintf(stderr, "docgen: attempt to embed unknown macro_function '%s'\n", embed.name);
            exit(EXIT_FAILURE);
        }

        macro_function = macro_functions->contents[macro_function_index];

        /* Write the comment if there is one, and its allowed */
        if((strlen(macro_function.brief) != 0) && project.macro_function_briefs == 1)
            fprintf(location, "/* %s */", macro_function.brief);

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
    }

    fprintf(location, "```\n\n");
}

static void embed_functions(struct DocgenProject project,
                            struct DocgenFunctions *functions,
                            FILE *location) {
    int index = 0;

    fprintf(location, "```c\n");

    /* Function Embeds */
    for(index = 0; index < carray_length(project.embeds); index++) {
        int function_index = 0;
        int parameter_index = 0;
        struct DocgenFunction function;
        struct DocgenProjectEmbed embed = project.embeds->contents[index];

        if(embed.type != DOCGEN_PROJECT_EMBED_FUNCTION)
            continue;

        function_index = carray_find(functions, embed.name, function_index, FUNCTION);

        if(function_index == -1) {
            fprintf(stderr, "docgen: attempt to embed unknown function '%s'\n", embed.name);
            exit(EXIT_FAILURE);
        }

        function = functions->contents[function_index];

        /* Write the comment if there is one, and its allowed */
        if((strlen(function.brief) != 0) && project.function_briefs == 1)
            fprintf(location, "/* %s */\n", function.brief);

        /* "void" return type */
        if(function.return_data.return_type[0] == '\0')
            strcat(function.return_data.return_type, "void ");

        /* Function signature
         * Non-pointer types should not require a space after them to produce
         * the space, but asterisk attached to type instead of name should.
        */
        if(strchr(function.return_data.return_type, '*') == NULL)
            fprintf(location, "%s %s(", function.return_data.return_type, function.name);
        else
            fprintf(location, "%s%s(", function.return_data.return_type, function.name);

        /* Generate the arguments and parameters */
        for(parameter_index = 0; parameter_index < carray_length(function.parameters); parameter_index++) {
            int is_ptr = 0;
            struct DocgenFunctionParameter parameter;

            parameter = function.parameters->contents[parameter_index];

            if(strchr(parameter.type, '*') != NULL)
                is_ptr = 1;

            /* Decide whether or not to display the trailing comma */
            if(parameter_index == carray_length(function.parameters) - 1) {
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
        if(carray_length(function.parameters) == 0)
            fprintf(location, "%s", "void");
            
        fprintf(location, "%s", ");\n");
    }

    fprintf(location, "%s", "```\n\n");
}

static void embed_structures_recurse(struct DocgenProject project,
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
        embed_structures_recurse(project, structure.nested, location, longest,
                                 depth + 1);

        /* Add manual padding */
        for(pad_index = 0; pad_index < depth * 4; pad_index++) {
            fprintf(location, "%c", ' ');
        } 

        fprintf(location, "} %s;\n", structure.name);
    }
}

static void embed_structures(struct DocgenProject project,
                             struct DocgenStructures *structures,
                             FILE *location) {
    int index = 0;
    int embeds = 0;
    int number_of_embeds = number_of_embedded_types(project.embeds, DOCGEN_PROJECT_EMBED_STRUCTURE);
    int longest_field = docgen_get_longest_field(structures, 0);

    fprintf(location, "%s", "```c\n");

    for(index = 0; index < carray_length(project.embeds); index++) {
        int field_index = 0;
        int structure_index = 0;
        struct DocgenStructure structure;
        struct DocgenProjectEmbed embed = project.embeds->contents[index];

        if(embed.type != DOCGEN_PROJECT_EMBED_STRUCTURE)
            continue;

        embeds++;

        structure_index = carray_find(structures, embed.name, structure_index, STRUCTURE);
        liberror_is_number(docgen_project_manpage_embed_structures, structure_index, "%i", -1);
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

        embed_structures_recurse(project, structure.nested, location, longest_field, 1);

        fprintf(location, "%s", "};\n");

        /* Add an extra new line */
        if(embeds < number_of_embeds)
            fprintf(location, "%c", '\n');
    }

    fprintf(location, "%s", "```\n\n");
}












static void head(FILE *location, struct DocgenArguments arguments, struct DocgenProject project) {
    fprintf(location, "# %s.md\n\n", project.name);
    fprintf(location, "%s", "### NAME\n");
    fprintf(location, "%s - %s\n\n", project.name, project.brief);
}

static void synopsis(FILE *location, struct LibmatchCursor cursor,
                     struct DocgenArguments arguments, struct DocgenProject project) {
    int index = 0;
    struct DocgenMacros *macros = NULL;
    struct DocgenFunctions *functions = NULL;
    struct DocgenStructures *structures = NULL;
    struct DocgenMacroFunctions *macro_functions = NULL;

    /* Comment junk */
    const char *comment_start = docgen_get_comment_start(arguments);
    const char *comment_end = docgen_get_comment_end(arguments);

    macros = docgen_extract_macros(&cursor, comment_start, comment_end);
    functions = docgen_extract_functions(&cursor, comment_start, comment_end);
    structures = docgen_extract_structures(&cursor, comment_start, comment_end); 
    macro_functions = docgen_extract_macro_functions(&cursor, comment_start, comment_end); 

    fprintf(location, "%s", "### SYNOPSIS\n");
    fprintf(location, "%s\n", project.arguments);
    
    write_inclusions(arguments.inclusions);

    embed_constants(project, macros, location);
    embed_structures(project, structures, location);
    embed_functions(project, functions, location);
    embed_macro_functions(project, macro_functions, location);

    docgen_extract_macros_free(macros);
    docgen_extract_functions_free(functions);
    docgen_extract_structures_free(structures);
    docgen_extract_macro_functions_free(macro_functions);
}

static void evaluate_table(FILE *location, struct LibmatchCursor *cursor) {
    int index = 0;
    int separator = -1;
    char line[DOCGEN_LINE_LENGTH + 1] = "";

    libmatch_read_until(cursor, line, DOCGEN_LINE_LENGTH, "\n");

    /* Must be followed by a separator */
    if(strstarts(line, "sep: ") == 0) {
        fprintf(stderr, "docgen: table on line %i expected separator immediately following "
                "it of form '@sep: SEP_CHAR', got '%s'\n", cursor->line - 1, line);
        exit(EXIT_FAILURE);
    }

    separator = *(line + strlen("sep: "));

    libmatch_read_until(cursor, line, DOCGEN_LINE_LENGTH, "\n");
    fprintf(location, "%s", "| ");

    /* Dump the header */
    for(index = 0; line[index] != '\0'; index++) {
        int character = line[index];

        if(character == separator)
            fprintf(location, "%s", " | ");
        else
            fprintf(location, "%c", character);
    }

    fprintf(location, "%s", " |\n| ");

    /* Dump the hyphons */
    for(index = 0; line[index] != '\0'; index++) {
        int character = line[index];

        if(character == separator)
            fprintf(location, "%s", " | ");
        else
            fprintf(location, "%c", '-');
    }

    fprintf(location, "%s", " |\n");

    /* Start reading parts of the table */
    while(cursor->cursor < cursor->length) {
        libmatch_read_until(cursor, line, DOCGEN_LINE_LENGTH, "\n");

        /* Stop dumping the table */
        if(strcmp(line, "table") == 0)
            return;

        fprintf(location, "%s", "| ");

        /* Dump each section of the table */
        for(index = 0; line[index] != '\0'; index++) {
            int character = line[index];

            if(character == separator)
                fprintf(location, "%s", " | ");
            else
                fprintf(location, "%c", character);
        }

        fprintf(location, "%s", " |\n");
    }
}

static void description(FILE *location, struct DocgenArguments arguments, struct DocgenProject project) {
    char line[DOCGEN_LINE_LENGTH + 1] = "";
    struct LibmatchCursor description_cursor;

    description_cursor = libmatch_cursor_init(project.description, strlen(project.description));

    fprintf(location, "%s", "### DESCRIPTION\n");

    /* Iterate through each line */
    while(description_cursor.cursor < description_cursor.length) {
        libmatch_read_until(&description_cursor, line, DOCGEN_LINE_LENGTH, "\n");

        /* Start parsing a table */
        if(strcmp(line, "table") == 0) {
            evaluate_table(location, &description_cursor);

            continue;
        }

        fprintf(location, "%s\n</br>\n", line);
    }

    fprintf(location, "%s", "\n");
}

static void see_also(FILE *location, struct DocgenArguments arguments, struct DocgenProject project) {
    int index = 0;

    fprintf(location, "%s", "### SEE ALSO\n");

    if(carray_length(project.references) == 0) {
        fprintf(location, "%s", "This project has no references.\n");
        return;
    }

    while(index < carray_length(project.references)) {
        struct Reference reference = project.references->contents[index];

        fprintf(location, "%s(%s)", reference.manual, reference.section);

        /* Add a comma */
        if(index < (carray_length(project.references) - 1))
            fprintf(location, "%s", ", ");

        index++;
    }

    fprintf(location, "%c", '\n');
}

void docgen_project_markdown(struct DocgenArguments arguments, struct LibmatchCursor cursor, struct DocgenProject project) {
    int index = 0;
    FILE *location = NULL;
    char file_path[LIBPATH_MAX_PATH + 1];

    /* Open the file */
    docgen_create_file_path(arguments, project.name, file_path, LIBPATH_MAX_PATH);
    libpath_join_path(file_path, LIBPATH_MAX_PATH, "./doc/", project.name,
                      ".md", NULL);
    location = fopen(file_path, "w");

    /* Dump parts of the Markdown */
    head(location, arguments, project);
    synopsis(location, cursor, arguments, project);
    description(location, arguments, project);
    see_also(location, arguments, project);
}
