#ifndef CWARE_DOCGEN_EXTRACTOR_MACROS_H
#define CWARE_DOCGEN_EXTRACTOR_MACROS_H

#define DOCGEN_MACRO_NAME_LENGTH    64
#define DOCGEN_MACRO_VALUE_LENGTH   128
#define DOCGEN_MACRO_BRIEF_LENGTH   256

#define MACRO_TYPE  struct DocgenMacro
#define MACRO_HEAP  1
#define MACRO_COMPARE(cmp_a, cmp_b) \
    (strcmp(cmp_a.name, cmp_b) == 0)

#include "../../docgen.h"

/*
 * A macro in the source code of a program.
*/
struct DocgenMacro {
    int ifndef;
    char name[DOCGEN_MACRO_NAME_LENGTH + 1];
    char value[DOCGEN_MACRO_VALUE_LENGTH + 1];
    char brief[DOCGEN_MACRO_BRIEF_LENGTH + 1];
};

/*
 * An array of macros.
*/
struct DocgenMacros {
    CARRAY_COUNTER_TYPE length;
    CARRAY_COUNTER_TYPE capacity;
    struct DocgenMacro *contents;
};

void docgen_extract_macros_free(struct DocgenMacros *macros);
struct DocgenMacros *docgen_extract_macros(struct LibmatchCursor *cursor,
                                           const char *comment_start,
                                           const char *comment_end);

#endif
