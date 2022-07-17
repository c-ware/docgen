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
 * The backend for converting 'project' tokens into manual pages.
*/

#include "../projects.h"

static void embed_constants(struct DocgenProject project,
                            struct DocgenMacros *macros,
                            FILE *location) {
    int index = 0;

    fprintf(location, "%s", "\n.br\n");

    /* Constant Embeds */
    for(index = 0; index < carray_length(project.embeds); index++) {
        int macro_index = 0;
        struct DocgenMacro macro;
        struct Embed embed = project.embeds->contents[index];

        if(embed.type != DOCGEN_EMBED_CONSTANT)
            continue;

        macro_index = carray_find(macros, embed.name, macro_index, MACRO);
        macro = macros->contents[macro_index];

        /* Write the comment if there is one */
        if(strlen(macro.brief) != 0)
            fprintf(location, "/* %s */\n.br\n", macro.brief);
        
        /* Wrap the #define in an #ifndef */
        if(macro.ifndef == 1)
            fprintf(location, "#ifndef %s\n.br\n", macro.name);

        fprintf(location, "#define %s %s\n.br\n", macro.name, macro.value);

        if(macro.ifndef == 1)
            fprintf(location, "%s", "#endif\n.br\n");

        fprintf(location, "%s", "\n");
    }
}

static void embed_macro_functions(struct DocgenProject project,
                            struct DocgenMacroFunctions *macro_functions,
                            FILE *location) {
    int index = 0;

    fprintf(location, "%s", "\n.br\n");

    /* Macro Function Embeds */
    for(index = 0; index < carray_length(project.embeds); index++) {
        int parameter_index = 0;
        int macro_function_index = 0;
        struct DocgenMacroFunction macro_function;
        struct Embed embed = project.embeds->contents[index];

        if(embed.type != DOCGEN_EMBED_MACRO_FUNCTION)
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

        /* "#define " */
        fprintf(location, "%s", "\n.br\n\\fB#define ");

        /* Name */
        fprintf(location, "\n%s(", macro_function.name);

        /* Generate the arguments and parameters */
        for(parameter_index = 0; parameter_index < carray_length(macro_function.parameters); parameter_index++) {
            struct DocgenMacroFunctionParameter parameter;

            parameter = macro_function.parameters->contents[parameter_index];

            /* Decide whether or not to display the trailing comma */
            if(parameter_index == carray_length(macro_function.parameters) - 1) {
                fprintf(location, "\\fI%s\\fB", parameter.name);
            } else {
                fprintf(location, "\\fI%s\\fB, ", parameter.name);
            }
        }
        fprintf(location, "%s", ");\n.br\\fR");
    }

    fprintf(location, "%s", "\n.br\n");
}

static void embed_functions(struct DocgenProject project,
                            struct DocgenFunctions *functions,
                            FILE *location) {
    int index = 0;

    fprintf(location, "%s", "\n.br\n");

    /* Function Embeds */
    for(index = 0; index < carray_length(project.embeds); index++) {
        int function_index = 0;
        int parameter_index = 0;
        struct DocgenFunction function;
        struct Embed embed = project.embeds->contents[index];

        if(embed.type != DOCGEN_EMBED_FUNCTION)
            continue;

        function_index = carray_find(functions, embed.name, function_index, FUNCTION);

        if(function_index == -1) {
            fprintf(stderr, "docgen: attempt to embed unknown function '%s'\n", embed.name);
            exit(EXIT_FAILURE);
        }

        function = functions->contents[function_index];

        /* Write the comment if there is one, and its allowed */
        if((strlen(function.brief) != 0) && project.function_briefs == 1)
            fprintf(location, "/* %s */\n.br", function.brief);

        /* "void" return type */
        if(function.return_data.return_type[0] == '\0')
            strcat(function.return_data.return_type, "void ");

        /* Function signature
         * Non-pointer types should not require a space after them to produce
         * the space, but asterisk attached to type instead of name should.
        */
        if(strchr(function.return_data.return_type, '*') == NULL)
            fprintf(location, "\n\\fB%s %s(", function.return_data.return_type,
                    function.name);
        else
            fprintf(location, "\n\\fB%s%s(", function.return_data.return_type,
                    function.name);

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
                    fprintf(location, "%s \\fI%s\\fB", parameter.type, parameter.name);
                } else {
                    fprintf(location, "%s\\fI%s\\fB", parameter.type, parameter.name);
                }
            } else {
                if(is_ptr == 0) {
                    fprintf(location, "%s \\fI%s\\fB, ", parameter.type,
                            parameter.name);
                } else {
                    fprintf(location, "%s\\fI%s\\fB, ", parameter.type,
                            parameter.name);
                }
            }
        }

        /* If theres no parameters, simply do void */
        if(carray_length(function.parameters) == 0)
            fprintf(location, "%s", "void");
            
        fprintf(location, "%s", ");\n.br\\fR");
    }

    fprintf(location, "%s", "\n.br\n");
}

static void embed_structures_recurse(struct DocgenProject project,
                                     struct DocgenStructures *structures,
                                     FILE *location, int longest, int depth) {
    int index = 0;

    for(index = 0; index < carray_length(structures); index++) {
        int field_index = 0;
        struct DocgenStructure structure = structures->contents[index];

        fprintf(location, "%s", "struct {\n.br\n");
        fprintf(location, "%s", ".RS  0.4i\n.br\n");

        /* Display all fields */
        for(field_index = 0; field_index < carray_length(structure.fields); field_index++) {
            struct DocgenStructureField field;

            if(structure.fields == NULL)
                break;

            field = structure.fields->contents[field_index];

            /* Should be a space if there is no pointer, so regular
             * fields will not have an asterisk. Lack of pointer should
             * also have a shorter length since the length calculation
             * does not account for the artifical insertion of a space
             * between field and type names in situations without pointers. */
            if(strchr(field.type, '*') == NULL) {
                fprintf(location, "%s %s; ", field.type, field.name);
                docgen_do_padding(field, longest - 1, depth, location);
            } else {
                fprintf(location, "%s%s; ", field.type, field.name);
                docgen_do_padding(field, longest, depth, location);
            }


            if(strlen(field.description) != 0)
                fprintf(location, "/* %s */\n.br\n", field.description);
        }

        /* Display nested structures */
        embed_structures_recurse(project, structure.nested, location, longest,
                                 depth + 1);

        fprintf(location, "%s", ".RE\n.br\n");
        fprintf(location, "} %s;\n.br\n", structure.name);
    }
}

static void embed_structures(struct DocgenProject project,
                             struct DocgenStructures *structures,
                             FILE *location) {
    int index = 0;
    int longest_field = docgen_get_longest_field(structures, 0);

    fprintf(location, "%s", "\n.br\n");

    for(index = 0; index < carray_length(project.embeds); index++) {
        int field_index = 0;
        int structure_index = 0;
        struct DocgenStructure structure;
        struct Embed embed = project.embeds->contents[index];

        if(embed.type != DOCGEN_EMBED_STRUCTURE)
            continue;

        structure_index = carray_find(structures, embed.name, structure_index, STRUCTURE);
        liberror_is_number(docgen_project_manpage_embed_structures, structure_index, "%i", -1);
        structure = structures->contents[structure_index];

        if(strlen(structure.brief) != 0)
            fprintf(location, "/* %s */\n.br\n", structure.brief);

        fprintf(location, "struct %s {\n.br\n", structure.name);
        fprintf(location, "%s", ".RS  0.4i\n.br\n");

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
                fprintf(location, "%s %s; ", field.type, field.name);
                docgen_do_padding(field, longest_field - 1, 0, location);
            } else {
                fprintf(location, "%s%s; ", field.type, field.name);
                docgen_do_padding(field, longest_field, 0, location);
            }

            if(strlen(field.description) != 0)
                fprintf(location, "/* %s */\n.br\n", field.description);
        }

        embed_structures_recurse(project, structure.nested, location, longest_field, 1);

        fprintf(location, "%s", ".RE\n.br\n");
        fprintf(location, "%s", "};\n.br\n");
    }
}


static void header(struct DocgenArguments arguments,
                   struct DocgenProject project,
                   FILE *location) {
    liberror_is_null(header, location);

    fprintf(location, ".TH \"%s\" \"%s\" \"%s\" \"\" \"%s\"\n",
            project.name, arguments.section, arguments.date,
            arguments.title);
}

static void name(struct DocgenProject project, FILE *location) {
    fprintf(location, "%s", ".SH NAME\n");
    fprintf(location, "%s - %s\n", project.name, project.brief);
}

static void description_table(struct LibmatchCursor *cursor, FILE *location,
                              int separator) {
    int index = 0;
    int columns = 0;
    char table_line[DOCGEN_LINE_LENGTH + 1];

    fprintf(location, "%s", ".TS\n");
    fprintf(location, "tab(%c);\n", separator);

    /* Determine how many columns there are */
    libmatch_read_until(cursor, table_line, DOCGEN_LINE_LENGTH, "\n");
    libmatch_cursor_getch(cursor);

    columns = strcount(table_line, ";") + 1;

    /* Make the formatting information */
    for(index = 0; index < columns; index++) {
        fprintf(location, "%s", "l ");
    }

    fprintf(location, "%c", '\n');

    for(index = 0; index < columns; index++) {
        fprintf(location, "%s", "_ ");
    }

    fprintf(location, "%c", '\n');

    for(index = 0; index < columns; index++) {
        fprintf(location, "%s", "l ");
    }

    fprintf(location, "%c", '\n');
    fprintf(location, "%s", ".\n");

    /* Dump the contents of the manual until a line starting with `table`
     * is found */
    while(strcmp(table_line, "table") != 0) {
        fprintf(location, "%s\n", table_line);
        libmatch_read_until(cursor, table_line, DOCGEN_LINE_LENGTH, "\n");
        libmatch_cursor_getch(cursor);
    }

    fprintf(location, "%s", ".TE\n");
}

static void description_eval(FILE *location, char *buffer) {
    struct LibmatchCursor cursor = libmatch_cursor_init(buffer, strlen(buffer));

    /* Display the description, and any tables. */
    while(cursor.cursor < cursor.length) { 
        /* Saving the cursor position here because if a line starts with
         * any of the first letters of "table", even though it might not be
         * "table", then it will not display those characters. */
        int saved_position = cursor.cursor;

        libmatch_cursor_enable_pushback(&cursor);

        /* Table starts at the first line */
        if(libmatch_string_expect(&cursor, "table\n") == 1) {
            int separator = -1;

            /* Read the separator */
            /* TODO: produce an error here */
            if(libmatch_string_expect(&cursor, "sep: ") == 0) {

            }

            separator = libmatch_cursor_getch(&cursor);
            libmatch_next_line(&cursor);

            description_table(&cursor, location, separator);
        } else
            /* Restore cursor position if the match failed */
            cursor.cursor = saved_position;

        libmatch_cursor_disable_pushback(&cursor);
            
        libmatch_exec_until(&cursor, '\n') {
            fprintf(location, "%c", libmatch_cursor_getch(&cursor));
        }


        fprintf(location, "%c", '\n');

        /* Go past the new line */
        libmatch_cursor_getch(&cursor);
    }
}

static void description(struct DocgenProject project, FILE *location) {
    int index = 0;

    fprintf(location, "%s", ".SH DESCRIPTION\n");
    description_eval(location, project.description);

    if(project.categories->length == 0)
        return;

    /* Sub-categories of the main page */
    fprintf(location, "%s", ".TS\n");
    fprintf(location, "%s", "tab(;);\n");
    fprintf(location, "%s", "lb l\n");
    fprintf(location, "%s", "_ _\n");
    fprintf(location, "%s", "lb l\n");
    fprintf(location, "%s", ".\n");
    fprintf(location, "%s", "Manual;Description\n");

    for(index = 0; index < carray_length(project.categories); index++) {
        struct DocgenProjectCategory category = project.categories->contents[index];

        fprintf(location, "%s;%s\n", category.name, category.description);
    }

    fprintf(location, "%s", ".TE\n");
}

static void see_also(struct DocgenProject project, FILE *location) {
    int index = 0;

    /* See also */
    fprintf(location, "%s", ".SH SEE ALSO\n");

    if(carray_length(project.references) == 0)
        fprintf(location, "%s", "This project has no references.\n");
    else {
        struct Reference reference;

        index = 0;

        while(index < carray_length(project.references) - 1) {
            reference = project.references->contents[index];

            fprintf(location, "\\fB%s\\fR(%s), ", reference.manual,
                    reference.section);
            index++;
        }

        reference = project.references->contents[index];
        
        /* Print the last one */
        fprintf(location, "\\fB%s\\fR(%s)\n", reference.manual,
                reference.section);
        fprintf(location, "%s", ".br\n");
    }
}

static void synopsis(struct DocgenArguments arguments,
                     struct DocgenProject project,
                     struct LibmatchCursor cursor,
                     FILE *location) {
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

    fprintf(location, "%s", ".SH SYNOPSIS\n");
    fprintf(location, "%s\n", project.arguments);
    
    /* Inclusions */
    for(index = 0; index < carray_length(arguments.inclusions); index++) {
        struct Inclusion inclusion = arguments.inclusions->contents[index];

        if(inclusion.type == DOCGEN_INCLUSION_LOCAL) {
            fprintf(location, ".B \"#include \"\"%s\"\"\"\n.br\n", inclusion.path);
        } else if(inclusion.type == DOCGEN_INCLUSION_SYSTEM) {
            fprintf(location, ".B #include <%s>\n.br\n\n", inclusion.path);
        }
    }

    fprintf(location, "%s", ".br\n\n");

    embed_constants(project, macros, location);
    embed_structures(project, structures, location);
    embed_functions(project, functions, location);
    embed_macro_functions(project, macro_functions, location);

    docgen_extract_macros_free(macros);
    docgen_extract_functions_free(functions);
    docgen_extract_structures_free(structures);
    docgen_extract_macro_functions_free(macro_functions);
}

void docgen_project_manpage(struct DocgenArguments arguments,
                            struct LibmatchCursor cursor,
                            struct DocgenProject project) {
    FILE *location = NULL;
    char file_path[LIBPATH_MAX_PATH + 1];

    /* Open the file */
    docgen_create_file_path(arguments, project.name, file_path, LIBPATH_MAX_PATH);
    libpath_join_path(file_path, LIBPATH_MAX_PATH, "./doc/", project.name,
                      ".", arguments.section, NULL);
    location = fopen(file_path, "w");

    liberror_is_null(docgen_project_manpage, location);

    header(arguments, project, location);
    name(project, location);
    synopsis(arguments, project, cursor, location);
    description(project, location);
    see_also(project, location);

    fclose(location);
}
