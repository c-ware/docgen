#ifndef CWARE_DOCGEN_PROJECTS_H
#define CWARE_DOCGEN_PROJECTS_H

#include "../../docgen.h"
#include "../../extractors/macros/macros.h"
#include "../../extractors/functions/functions.h"
#include "../../extractors/structures/structures.h"
#include "../../extractors/macro_functions/macro_functions.h"

/* Limits */
#define DOCGEN_PROJECT_NAME_LENGTH                  128
#define DOCGEN_PROJECT_BRIEF_LENGTH                 128
#define DOCGEN_PROJECT_CATEGORY_LENGTH              128

#define DOCGEN_PROJECT_DESCRIPTION_LENGTH           8192 + 1

#define DOCGEN_PROJECT_EMBED_NAME_LENGTH            128
#define DOCGEN_PROJECT_EMBED_TYPE_LENGTH            32

#define DOCGEN_PROJECT_CATEGORY_DESCRIPTION_LENGTH  128
#define DOCGEN_PROJECT_CATEGORY_NAME_LENGTH         128

#define DOCGEN_PROJECT_ARGUMENTS_LENGTH             2048

#define DOCGEN_PROJECT_SETTING_LENGTH               32

/* Data structure properties */
#define CATEGORY_TYPE   struct DocgenProjectCategory
#define CATEGORY_HEAP   1
#define CATEGORY_FREE(value)

#define EMBED_TYPE struct DocgenProjectEmbed
#define EMBED_HEAP 1
#define EMBED_FREE(value)

/* Enumerations */
#define DOCGEN_PROJECT_EMBED_UNKNOWN        0
#define DOCGEN_PROJECT_EMBED_FUNCTION       1
#define DOCGEN_PROJECT_EMBED_STRUCTURE      2
#define DOCGEN_PROJECT_EMBED_CONSTANT       3
#define DOCGEN_PROJECT_EMBED_MACRO_FUNCTION 4

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
 * Represents a token to load into the manual page. This could
 * be a function, structure, constant, etc.
*/
struct DocgenProjectEmbed {
    char name[DOCGEN_PROJECT_EMBED_NAME_LENGTH + 1];
    int type;
};

/*
 * An array of embeds.
*/
struct DocgenProjectEmbeds {
    CARRAY_COUNTER_TYPE length;
    CARRAY_COUNTER_TYPE capacity;
    struct DocgenProjectEmbed *contents;
};

/*
 * Represents an abstract view of a 'project.'
*/
struct DocgenProject {
    char name[DOCGEN_PROJECT_NAME_LENGTH + 1];
    char brief[DOCGEN_PROJECT_BRIEF_LENGTH + 1];
    char description[DOCGEN_PROJECT_DESCRIPTION_LENGTH + 1];
    char arguments[DOCGEN_PROJECT_ARGUMENTS_LENGTH + 1];

    /* Configuration */
    int function_briefs;
    int macro_function_briefs;

    /* Arrays */
    struct DocgenProjectCategorys *categories;
    struct DocgenProjectEmbeds *embeds;
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

/* IBM VM/CMS and MVS Document Composition Facility */
void docgen_project_dcfscript(struct DocgenArguments arguments,
                            struct LibmatchCursor cursor,
                            struct DocgenProject project);

/* IBM VM/CMS HELPCMS command help document */
void docgen_project_helpcms(struct DocgenArguments arguments,
                            struct LibmatchCursor cursor,
                            struct DocgenProject project);

/* Documentation generation */
void docgen_project_generate(struct DocgenArguments arguments, FILE *file);
void docgen_project_category_generate(struct DocgenArguments arguments, FILE *file);

#endif
