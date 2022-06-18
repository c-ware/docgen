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
 * The backend for converting 'project' tokens into help files for the 
 * IBM VM/CMS operating system. This is for the VM/SP Release 5 and newer
 * help system.
*/

#include "../projects.h"

void docgen_project_helpcms_embed_constants(struct DocgenProject project, struct DocgenMacros *macros, FILE *location) {
    int index = 0;

    /* Constant Embeds */
    for(index = 0; index < carray_length(project.embeds); index++) {
        int macro_index = 0;
        struct DocgenMacro macro;
        struct DocgenProjectEmbed embed = project.embeds->contents[index];

        if(embed.type != DOCGEN_PROJECT_EMBED_CONSTANT)
            continue;

        macro_index = carray_find(macros, embed.name, macro_index, MACRO);
        macro = macros->contents[macro_index];

        /* Write the comment if there is one */
        if(strlen(macro.brief) != 0)
            fprintf(location, "    /* %s */\n", macro.brief);
        
        /* Wrap the #define in an #ifndef */
        if(macro.ifndef == 1)
            fprintf(location, "    #ifndef %s\n", macro.name);

        fprintf(location, "    #define %s %s\n", macro.name, macro.value);

        if(macro.ifndef == 1)
            fprintf(location, "%s", "    #endif\n");

        fprintf(location, "%s", "\n");
    }
}

void docgen_project_helpcms_embed_structures_recurse(struct DocgenProject project,
                                                     struct DocgenStructures *structures,
                                                     FILE *location) {
    int index = 0;

    for(index = 0; index < carray_length(structures); index++) {
        int field_index = 0;
        struct DocgenStructure structure = structures->contents[index];

        fprintf(location, "%s", "        struct {\n");

        /* Display all fields */
        for(field_index = 0; field_index < carray_length(structure.fields); field_index++) {
            struct DocgenStructureField field;

            if(structure.fields == NULL)
               break;

            field = structure.fields->contents[field_index];

            fprintf(location, "            %s %s; ", field.type, field.name);

            if(strlen(field.description) != 0)
                fprintf(location, "            /* %s */\n", field.description);
        }

        /* Display nested structures */
        for(field_index = 0; field_index < carray_length(structure.nested); field_index++) {
            if(structure.nested == NULL)
               break;

            docgen_project_helpcms_embed_structures_recurse(project,
                                                            structure.nested,
                                                            location);
        }

        fprintf(location, "        } %s;\n", structure.name);
    }
}

void docgen_project_helpcms_embed_structures(struct DocgenProject project, struct DocgenStructures *structures, FILE *location) {
    int index = 0;

    for(index = 0; index < carray_length(project.embeds); index++) {
        int field_index = 0;
        int structure_index = 0;
        struct DocgenStructure structure;
        struct DocgenProjectEmbed embed = project.embeds->contents[index];

        if(embed.type != DOCGEN_PROJECT_EMBED_STRUCTURE)
            continue;

        structure_index = carray_find(structures, embed.name, structure_index, STRUCTURE);
        liberror_is_number(docgen_project_helpcms_embed_structures, structure_index, "%i", -1);
        structure = structures->contents[structure_index];

        if(strlen(structure.brief) != 0)
            fprintf(location, "    /* %s */\n", structure.brief);

        fprintf(location, "    struct %s {\n", structure.name);

        /* Display all fields */
        for(field_index = 0; field_index < carray_length(structure.fields); field_index++) {
            struct DocgenStructureField field;

            field = structure.fields->contents[field_index];

            fprintf(location, "        %s %s; ", field.type, field.name);

            if(strlen(field.description) != 0)
                fprintf(location, "        /* %s */\n", field.description);
        }

        /* Display nested structures */
        for(field_index = 0; field_index < carray_length(structure.nested); field_index++) {
            docgen_project_helpcms_embed_structures_recurse(project,
                                                            structure.nested,
                                                            location);
        }

        fprintf(location, "    %s", "};\n");
    }
}


void docgen_project_helpcms_header(struct DocgenArguments arguments, struct DocgenProject project, FILE *location) {
    /* fprintf(location, ".CS 2 ON\n(c) Copyright <none>\n.cm (adapted)\n\n"); */
    /* Note -- disabled because this is pretty much useless. */
}

void docgen_project_helpcms_name(struct DocgenProject project, FILE *location) {
    fprintf(location, "¢|%s\n\n", project.name);
}

void docgen_project_helpcms_description_table(struct LibmatchCursor *cursor,
                                              FILE *location, int separator) {
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

void docgen_project_helpcms_description_eval(FILE *location, char *buffer) {
    char table_line[DOCGEN_LINE_LENGTH + 1];
    char table_buffer[DOCGEN_TABLE_LENGTH + 1];
    struct LibmatchCursor cursor = libmatch_cursor_init(buffer, strlen(buffer));

    /* Display the description, and any tables. */
    while(cursor.cursor < cursor.length) { 
        int character = -1;

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

            docgen_project_helpcms_description_table(&cursor, location, separator);
        } else
            /* Restore cursor position if the match failed */
            cursor.cursor = saved_position;

        libmatch_cursor_disable_pushback(&cursor);
            
        libmatch_exec_until(&cursor, '\n') {
            fprintf(location, "%c", libmatch_cursor_getch(&cursor));
        }


        fprintf(location, "    %c", '\n');

        /* Go past the new line */
        libmatch_cursor_getch(&cursor);
    }
}

void docgen_project_helpcms_description(struct DocgenProject project, FILE *location) {
    int index = 0;

    fprintf(location, "%s", ".CS 2 OFF\n.CS 2 ON\n¢|Description¢%\n");
    docgen_project_helpcms_description_eval(location, project.description);

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

void docgen_project_helpcms_see_also(struct DocgenProject project, FILE *location) {
    int index = 0;

    /* See also */
    fprintf(location, "%s", ".CS 2 OFF\n.CS 2 ON\n¢|See Also¢%\n");

    if(carray_length(project.references) == 0)
        fprintf(location, "%s", "This project has no references.\n");
    else {
        struct Reference reference;

        index = 0;

        while(index < carray_length(project.references) - 1) {
            reference = project.references->contents[index];

            fprintf(location, "    %s (%s)", reference.manual,
                    reference.section);
            index++;
        }

        reference = project.references->contents[index];
        
        /* Print the last one */
        fprintf(location, "    %s (%s)", reference.manual,
                reference.section);
        fprintf(location, "    %s", "\n");
    }
}

void docgen_project_helpcms_synopsis(struct DocgenArguments arguments, struct DocgenProject project,
                                     struct LibmatchCursor cursor, FILE *location) {
    int index = 0;
    struct DocgenMacros *macros = NULL;
    struct DocgenStructures *structures = NULL;

    /* Comment junk */
    const char *comment_start = docgen_get_comment_start(arguments);
    const char *comment_end = docgen_get_comment_end(arguments);

    macros = docgen_extract_macros(&cursor, comment_start, comment_end);
    structures = docgen_extract_structures(&cursor, comment_start, comment_end);

    fprintf(location, "%s", ".CS 2 OFF\n.CS 2 ON\n¢|Synopsis¢%\n");
    fprintf(location, "%s\n", project.arguments);
    
    /* Inclusions */
    for(index = 0; index < carray_length(arguments.inclusions); index++) {
        struct Inclusion inclusion = arguments.inclusions->contents[index];

        if(inclusion.type == DOCGEN_INCLUSION_LOCAL) {
            fprintf(location, "    #include \"%s\"\"\"\n\n", inclusion.path);
        } else if(inclusion.type == DOCGEN_INCLUSION_SYSTEM) {
            fprintf(location, "    #include <%s>\n\n\n", inclusion.path);
        }
    }

    docgen_project_helpcms_embed_constants(project, macros, location);
    docgen_project_helpcms_embed_structures(project, structures, location);

    docgen_extract_macros_free(macros);
    docgen_extract_structures_free(structures);
}

void docgen_project_helpcms(struct DocgenArguments arguments,
                            struct LibmatchCursor cursor,
                            struct DocgenProject project) {
    FILE *location = NULL;
    char file_path[LIBPATH_MAX_PATH + 1];

    /* Open the file */
    docgen_create_file_path(arguments, project.name, file_path, LIBPATH_MAX_PATH);
    libpath_join_path(file_path, LIBPATH_MAX_PATH, "./doc/", project.name,
                      ".", arguments.section, NULL);
    location = fopen(file_path, "w");

    docgen_project_helpcms_header(arguments, project, location);
    docgen_project_helpcms_name(project, location);
    docgen_project_helpcms_synopsis(arguments, project, cursor, location);
    docgen_project_helpcms_description(project, location);
    docgen_project_helpcms_see_also(project, location);

    fclose(location);
}
