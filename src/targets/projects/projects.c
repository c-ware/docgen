/*
 * This file is used to store functions related to parsing "project"
 * pages. A project page serves as the central place that describes
 * the project.
*/

#include "projects.h"

/*
 * Generate code to extract a field line.
*/
#define docgen_project_field_line(tag, length, buffer)  \
    if(strcmp(tag_name.name, tag) == 0)                 \
        docgen_extract_field_line(tag, length, cursor.line, new_tag.line, new_project.buffer);

void docgen_project_format(struct DocgenArguments arguments,
                           struct LibmatchCursor cursor,
                           struct DocgenProject project) {

    if(strcmp(arguments.format, "manpage") == 0)
        docgen_project_manpage(arguments, cursor, project);
    else if(strcmp(arguments.format, "dcfscript") == 0)
        docgen_project_dcfscript(arguments, cursor, project);
    else if(strcmp(arguments.format, "helpcms") == 0) 
        docgen_project_helpcms(arguments, cursor, project);
    else {
        fprintf(stderr, "docgen: unknown format for projects '%s'\n", arguments.format);
        exit(EXIT_FAILURE);
    }
}

struct DocgenProject docgen_parse_project_comment(struct LibmatchCursor cursor,
                                                  const char *comment_start,
                                                  const char *comment_end) {
    int iteration = 0;
    struct DocgenProject new_project;

    memset(&new_project, 0, sizeof(struct DocgenProject));

    /* Prepare arrays */
    new_project.references = carray_init(new_project.references, REFERENCE);
    new_project.categories = carray_init(new_project.categories, CATEGORY);
    new_project.embeds = carray_init(new_project.embeds, EMBED);

    while(1) {
        struct DocgenTagName tag_name;
        struct DocgenTag new_tag = docgen_tag_next(&cursor, comment_start,
                                                   comment_end);

        iteration++;
        memset(&tag_name, 0, sizeof(struct DocgenTagName));

        /* Handle errors from the tag parser */
        tag_error(&new_tag, &cursor);

        /* Extract the tag name */
        tag_name = docgen_tag_name(new_tag);

        /* Handle errors from the tag name extraction */
        switch(tag_name.status) {
            case DOCGEN_TAG_STATUS_FULL:
                fprintf(stderr, "docgen: name of tag on line %i is too long. maximum size of %i\n",
                        cursor.line, DOCGEN_TAG_NAME_LENGTH - 1);
                exit(EXIT_FAILURE);
        }

        /* Handle the tags */
        if(strcmp(tag_name.name, "name") == 0)
            docgen_extract_field_line("name", DOCGEN_PROJECT_NAME_LENGTH, cursor.line,
                                      new_tag.line, new_project.name);

        else if(strcmp(tag_name.name, "brief") == 0)
            docgen_extract_field_line("brief", DOCGEN_PROJECT_BRIEF_LENGTH, cursor.line,
                                      new_tag.line, new_project.brief);

        else if(strcmp(tag_name.name, "arguments") == 0)
            docgen_extract_field_block("arguments", DOCGEN_PROJECT_ARGUMENTS_LENGTH,
                                 &cursor, new_tag.line, new_project.arguments);

        else if(strcmp(tag_name.name, "description") == 0)
            docgen_extract_field_block("description", DOCGEN_PROJECT_DESCRIPTION_LENGTH,
                                 &cursor, new_tag.line, new_project.description);

        else if(strcmp(tag_name.name, "reference") == 0) {
            struct Reference new_reference;

            memset(&new_reference, 0, sizeof(struct Reference));
            new_reference = docgen_extract_reference(&cursor, new_tag);

            carray_append(new_project.references, new_reference, REFERENCE);
        }

        else if(strcmp(tag_name.name, "setting") == 0) {
            char setting_name[DOCGEN_PROJECT_SETTING_LENGTH + 1];

            memset(setting_name, 0, sizeof(setting_name));
            docgen_extract_field_line("setting", DOCGEN_PROJECT_SETTING_LENGTH, cursor.line,
                                       new_tag.line, setting_name);

            if(strcmp(setting_name, "func-briefs") == 0)
                new_project.function_briefs = 1;
            else if(strcmp(setting_name, "mfunc-briefs") == 0)
                new_project.macro_function_briefs = 1;
            else {
                fprintf(stderr, "docgen: unknown setting '%s' on line %i\n", setting_name, cursor.line);
                exit(EXIT_FAILURE);
            }
        }

        else if(strcmp(tag_name.name, "embed") == 0) {
            struct DocgenProjectEmbed new_embed;
            char embed_type[DOCGEN_PROJECT_EMBED_TYPE_LENGTH + 1];

            memset(embed_type, 0, sizeof(embed_type));
            memset(&new_embed, 0, sizeof(struct DocgenProjectEmbed));

            docgen_extract_field_line_arg("embed", new_tag.line,
                                   DOCGEN_PROJECT_EMBED_TYPE_LENGTH, embed_type,
                                   DOCGEN_PROJECT_EMBED_NAME_LENGTH, new_embed.name,
                                   cursor.line);

            if(strcmp(embed_type, "structure") == 0)
                new_embed.type = DOCGEN_PROJECT_EMBED_STRUCTURE;
            else if(strcmp(embed_type, "function") == 0)
                new_embed.type = DOCGEN_PROJECT_EMBED_FUNCTION;
            else if(strcmp(embed_type, "constant") == 0)
                new_embed.type = DOCGEN_PROJECT_EMBED_CONSTANT;
            else if(strcmp(embed_type, "macro_function") == 0)
                new_embed.type = DOCGEN_PROJECT_EMBED_MACRO_FUNCTION;
            else {
                fprintf(stderr, "docgen: unknown embed type '%s' at line %i\n", embed_type, cursor.line);
                exit(EXIT_FAILURE);
            }

            carray_append(new_project.embeds, new_embed, EMBED);
        } else {
            fprintf(stderr, "docgen: unknown tag '%s' in project extractor on line %i\n",
                    tag_name.name, cursor.line);
            exit(EXIT_FAILURE);
        }
   }
   
break_loop:

   return new_project;
}

void docgen_project_generate(struct DocgenArguments arguments, FILE *file) {
    struct DocgenProject project;
    struct LibmatchCursor cursor = libmatch_cursor_from_stream(file);    

    /* Comment junk */
    const char *comment_start = docgen_get_comment_start(arguments);
    const char *comment_end = docgen_get_comment_end(arguments);

    /* Find comments */
    while(cursor.cursor != cursor.length) {
        struct DocgenFunction new_function;

        if(libmatch_string_expect(&cursor, comment_start) == 0)
            continue;

        if(docgen_comment_is_type(&cursor, comment_start, comment_end, "project") == 0)
            continue;

        break;
    }

    /* No comment found-- produce an error */
    if(cursor.cursor == cursor.length) {
        fprintf(stderr, "%s", "docgen: could not find project comment in file.\n");
        fprintf(stderr, "%s", "Try 'docgen --help' for more information.\n");
        exit(EXIT_FAILURE);
    }

    project = docgen_parse_project_comment(cursor, comment_start, comment_end);
    docgen_project_format(arguments, cursor, project);

    /* Cleanup */
    libmatch_cursor_free(&cursor);
    carray_free(project.embeds, EMBED);
    carray_free(project.categories, CATEGORY);
    carray_free(project.references, REFERENCE);
}

void docgen_project_category_generate(struct DocgenArguments arguments, FILE *file) {
    int saved_position = 0;
    struct DocgenProject project;
    struct LibmatchCursor cursor = libmatch_cursor_from_stream(file);    

    /* Comment junk */
    const char *comment_start = docgen_get_comment_start(arguments);
    const char *comment_end = docgen_get_comment_end(arguments);

    /* Find comments */
    while(cursor.cursor != cursor.length) {
        struct DocgenFunction new_function;

        if(libmatch_string_expect(&cursor, comment_start) == 0)
            continue;

        if(docgen_comment_is_type(&cursor, comment_start, comment_end, "category") == 0)
            continue;

        break;
    }

    /* No comment found-- produce an error */
    if(cursor.cursor == cursor.length) {
        fprintf(stderr, "%s", "docgen: could not find category comment in file.\n");
        fprintf(stderr, "%s", "Try 'docgen --help' for more information.\n");
        exit(EXIT_FAILURE);
    }

    project = docgen_parse_project_comment(cursor, comment_start, comment_end);
    docgen_project_format(arguments, cursor, project);

    /* Cleanup */
    libmatch_cursor_free(&cursor);
    carray_free(project.embeds, EMBED);
    carray_free(project.categories, CATEGORY);
    carray_free(project.references, REFERENCE);
}
