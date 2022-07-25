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
 * This file is used to store functions related to parsing "project"
 * pages. A project page serves as the central place that describes
 * the project.
*/

#include "../../docgen.h"
#include "../../extractors/macros/macros.h"
#include "../../extractors/functions/functions.h"
#include "../../extractors/structures/structures.h"
#include "../../extractors/macro_functions/macro_functions.h"

#include "projects.h"

/*
 * Generate code to extract a field line.
*/
#define docgen_project_field_line(tag, length, buffer)  \
    if(strcmp(tag_name.name, tag) == 0)                 \
        docgen_extract_field_line(tag, length, cursor.line, new_tag.line, new_project.buffer);

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
        if(strcmp(tag_name.name, "name") == 0) {
            docgen_extract_field_line("name", new_project.name, DOCGEN_PROJECT_NAME_LENGTH,
                                      cursor.line, new_tag.line);
        } else if(strcmp(tag_name.name, "brief") == 0) {
            docgen_extract_field_line("brief", new_project.brief, DOCGEN_PROJECT_BRIEF_LENGTH,
                                      cursor.line, new_tag.line);
        } else if(strcmp(tag_name.name, "arguments") == 0) {
            docgen_extract_field_block("arguments", new_project.arguments,
                                       DOCGEN_PROJECT_ARGUMENTS_LENGTH, &cursor, new_tag.line);
        } else if(strcmp(tag_name.name, "description") == 0) {
            docgen_extract_field_block("description", new_project.description,
                                       DOCGEN_PROJECT_DESCRIPTION_LENGTH, &cursor, new_tag.line);
        } else if(strcmp(tag_name.name, "reference") == 0) {
            struct Reference new_reference;

            memset(&new_reference, 0, sizeof(struct Reference));
            new_reference = docgen_extract_reference(&cursor, new_tag);

            carray_append(new_project.references, new_reference, REFERENCE);
        } else if(strcmp(tag_name.name, "setting") == 0) {
            char setting_name[DOCGEN_PROJECT_SETTING_LENGTH + 1];

            memset(setting_name, 0, sizeof(setting_name));
            docgen_extract_field_line("setting", setting_name, DOCGEN_PROJECT_SETTING_LENGTH,
                                      cursor.line, new_tag.line);

            if(strcmp(setting_name, "func-briefs") == 0)
                new_project.function_briefs = 1;
            else if(strcmp(setting_name, "mfunc-briefs") == 0)
                new_project.macro_function_briefs = 1;
            else if(strcmp(setting_name, "struct-briefs") == 0)
                new_project.structure_briefs = 1;
            else if(strcmp(setting_name, "macro-briefs") == 0)
                new_project.macro_briefs = 1;
            else {
                fprintf(stderr, "docgen: unknown setting '%s' on line %i\n", setting_name, cursor.line);
                exit(EXIT_FAILURE);
            }

        } else if(strcmp(tag_name.name, "embed") == 0) {
            struct Embed new_embed;
            char embed_type[DOCGEN_EMBED_TYPE_LENGTH + 1];

            memset(embed_type, 0, sizeof(embed_type));
            memset(&new_embed, 0, sizeof(struct Embed));

            docgen_extract_field_line_arg("embed", embed_type, DOCGEN_EMBED_TYPE_LENGTH,
                                   new_embed.name, DOCGEN_EMBED_NAME_LENGTH,
                                   cursor.line, new_tag.line);

            if(strcmp(embed_type, "structure") == 0)
                new_embed.type = DOCGEN_EMBED_STRUCTURE;
            else if(strcmp(embed_type, "function") == 0)
                new_embed.type = DOCGEN_EMBED_FUNCTION;
            else if(strcmp(embed_type, "constant") == 0)
                new_embed.type = DOCGEN_EMBED_CONSTANT;
            else if(strcmp(embed_type, "macro_function") == 0)
                new_embed.type = DOCGEN_EMBED_MACRO_FUNCTION;
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

struct DocgenProject docgen_extract_project(struct LibmatchCursor *cursor,
                                            const char *comment_start,
                                            const char *comment_end) {
    struct DocgenProject project;
    struct LibmatchCursor cursor_copy = libmatch_cursor_init(cursor->buffer, cursor->length);    

    /* Project might not be found, so let's initialize it here incase there is no
     * project, as if we do not, the free function will try to release invalid
     * memory. We should also initialize variables like arguments with default
     * values, because when we go to write the contents, it might try to write
     * NULL if there is no project. */
    INIT_VARIABLE(project);

    /* Find comments */
    while(cursor_copy.cursor != cursor_copy.length) {
        if(libmatch_string_expect(&cursor_copy, comment_start) == 0)
            continue;

        if(docgen_comment_is_type(&cursor_copy, comment_start, comment_end, "project") == 0)
            continue;

        break;
    }

    /* No project was found. */
    if(cursor_copy.cursor >= cursor_copy.length)
        return project;

    project = docgen_parse_project_comment(cursor_copy, comment_start, comment_end);

    return project;
}

void docgen_extract_project_free(struct DocgenProject *project) {
    /* So, just to clarify a bit, we have to make sure these are not NULL
     * because a project very well might not be parsed. It's just too
     * different from the other extracted components. */

    if(project->embeds != NULL)
        carray_free(project->embeds, EMBED);

    if(project->categories != NULL)
        carray_free(project->categories, CATEGORY);

    if(project->references != NULL)
        carray_free(project->references, REFERENCE);
}
