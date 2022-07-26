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

#ifndef CWARE_DOCGEN_PROJECTS_H
#define CWARE_DOCGEN_PROJECTS_H

/* Limits */
#define DOCGEN_PROJECT_NAME_LENGTH                  128
#define DOCGEN_PROJECT_BRIEF_LENGTH                 128
#define DOCGEN_PROJECT_CATEGORY_LENGTH              128

#define DOCGEN_PROJECT_DESCRIPTION_LENGTH           8192 + 1
#define DOCGEN_PROJECT_EXAMPLE_LENGTH               8192 + 1
#define DOCGEN_PROJECT_NOTES_LENGTH                 2048 + 1

#define DOCGEN_PROJECT_CATEGORY_DESCRIPTION_LENGTH  128
#define DOCGEN_PROJECT_CATEGORY_NAME_LENGTH         128

#define DOCGEN_PROJECT_ARGUMENTS_LENGTH             2048

#define DOCGEN_PROJECT_SETTING_LENGTH               32

/* Data structure properties */
#define CATEGORY_TYPE   struct DocgenProjectCategory
#define CATEGORY_HEAP   1
#define CATEGORY_FREE(value)

/*
 * An array of categories in the project comment.
*/
struct DocgenProjectCategorys {
    CARRAY_COUNTER_TYPE length;
    CARRAY_COUNTER_TYPE capacity;
    struct DocgenProjectCategory *contents;
};

/*
 * Represents a category in a project comment.
*/
struct DocgenProjectCategory {
    char name[DOCGEN_PROJECT_CATEGORY_NAME_LENGTH + 1];
    char description[DOCGEN_PROJECT_CATEGORY_DESCRIPTION_LENGTH + 1];
};

/*
 * Represents an abstract view of a 'project.'
*/
struct DocgenProject {
    char name[DOCGEN_PROJECT_NAME_LENGTH + 1];
    char brief[DOCGEN_PROJECT_BRIEF_LENGTH + 1];
    char description[DOCGEN_PROJECT_DESCRIPTION_LENGTH + 1];
    char arguments[DOCGEN_PROJECT_ARGUMENTS_LENGTH + 1];
    char example[DOCGEN_PROJECT_EXAMPLE_LENGTH + 1];
    char notes[DOCGEN_PROJECT_NOTES_LENGTH + 1];

    /* Configuration */
    int macro_briefs;
    int function_briefs;
    int structure_briefs;
    int macro_function_briefs;

    /* Arrays */
    struct DocgenProjectCategorys *categories;
    struct Embeds *embeds;
    struct References *references;
};

/* Backend selectors */
void docgen_project_format(struct DocgenArguments arguments,
                            struct LibmatchCursor cursor,
                            struct DocgenProject project);

/* Backends */
/* UNIX nroff/troff manual page */
void docgen_project_manpage(struct DocgenArguments arguments,
                            struct LibmatchCursor cursor,
                            struct DocgenProject project);

/* Markdown */
void docgen_project_markdown(struct DocgenArguments arguments,
                             struct LibmatchCursor cursor,
                             struct DocgenProject project);

/* IBM VM/CMS and MVS Document Composition Facility */
void docgen_project_dcfscript(struct DocgenArguments arguments,
                            struct LibmatchCursor cursor,
                            struct DocgenProject project);

/* IBM VM/CMS HELPCMS command help document */
void docgen_project_helpcms(struct DocgenArguments arguments,
                            struct LibmatchCursor cursor,
                            struct DocgenProject project);

/* Documentation generation */
void docgen_extract_project_free(struct DocgenProject *project);
void docgen_project_generate(struct DocgenArguments arguments, FILE *file);
void docgen_project_category_generate(struct DocgenArguments arguments, FILE *file);
struct DocgenProject docgen_extract_project(struct LibmatchCursor *cursor,
                                            const char *comment_start,
                                            const char *comment_end);
#endif

