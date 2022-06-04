#ifndef CWARE_DOCGEN_H
#define CWARE_DOCGEN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tags/tags.h"
#include "carray/carray.h"
#include "libarg/libarg.h"
#include "libpath/libpath.h"
#include "liberror/liberror.h"
#include "libmatch/libmatch.h"
#include "libstring/libstring.h"
#include "common/common.h"

#define CWARE_DOCGEN_VERSION    "1.0.2"

/* Limits */
#define DOCGEN_MANUAL_NAME_LENGTH       64 + 1
#define DOCGEN_MANUAL_SECTION_LENGTH    64 + 1

#define DOCGEN_TABLE_LENGTH             2048
#define DOCGEN_LINE_LENGTH              1024

#define DOCGEN_BLOCK_LINE_LENGTH        1024 + 1

#define DOCGEN_INCLUSION_TYPE_LENGTH    32 + 1
#define DOCGEN_INCLUSION_PATH_LENGTH    128 + 1

/* Categories to parse */
#define DOCGEN_CATEGORY_UNKOWN      0
#define DOCGEN_CATEGORY_FUNCTION    1
#define DOCGEN_CATEGORY_PROJECT     2
#define DOCGEN_CATEGORY_CATEGORY    3

/* Supported languages */
#define DOCGEN_LANGUAGE_UNKNOWN 0
#define DOCGEN_LANGUAGE_C       1

/* From: single comment mode attempt
#define DOCGEN_LANGUAGE_PY      1
*/

/* Enumerations */
#define DOCGEN_INCLUSION_LOCAL      0
#define DOCGEN_INCLUSION_SYSTEM     1

/* Data structure properties */
#define INCLUDE_TYPE            struct Inclusion
#define INCLUDE_HEAP            1
#define INCLUDE_FREE(value)

#define REFERENCE_TYPE          struct Reference
#define REFERENCE_HEAP          1
#define REFERENCE_FREE(value)

/* Configuration */
#define DOCGEN_INDENTATION  4

/*
 * Represents an inclusion specified at the command line.
*/
struct Inclusion {
    int type;
    char path[DOCGEN_INCLUSION_PATH_LENGTH + 1];
};

/*
 * An array of inclusions.
*/
struct Inclusions {
    int length;
    int capacity;
    struct Inclusion *contents;
};

/*
 * Represents a 'reference' to another source of documentation. This is used
 * in the 'See Also' section.
*/
struct Reference {
    char manual[DOCGEN_MANUAL_NAME_LENGTH + 1];
    char section[DOCGEN_MANUAL_SECTION_LENGTH + 1];
};

/*
 * An array of references.
*/
struct References {
    int length;
    int capacity;
    struct Reference *contents;
};

/*
 * Arguments for docgen.
*/
struct DocgenArguments {
    char *category;
    char *source;
    char *format;

    /* Extra information */
    char *section;
    char *title;
    char *date;
    const char *language;

    /* Inclusions to load into each manual */
    struct Inclusions *inclusions;
};

/* Argument parsing and setup */
int main_enumerate(const char *category);
int main_enumerate_language(const char *language);
int main_parameters(const char *option);
void main_error(const char *option, int type, int expected, int got);
void main_extract_inclusions(int argc, char **argv, struct DocgenArguments *arguments);
struct DocgenArguments main_parse(int argc, char **argv);

#endif
