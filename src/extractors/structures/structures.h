#ifndef CWARE_DOCGEN_EXTRACTOR_STRUCTURES_H
#define CWARE_DOCGEN_EXTRACTOR_STRUCTURES_H

#define DOCGEN_STRUCTURE_NAME_LENGTH                64
#define DOCGEN_STRUCTURE_BRIEF_LENGTH               128
#define DOCGEN_PROJECT_SETTING_LENGTH               32

#define DOCGEN_STRUCTURE_FIELD_NAME_LENGTH          64
#define DOCGEN_STRUCTURE_FIELD_TYPE_LENGTH          64
#define DOCGEN_STRUCTURE_FIELD_DESCRIPTION_LENGTH   128

#define STRUCTURE_TYPE  struct DocgenStructure
#define STRUCTURE_HEAP  1
#define STRUCTURE_COMPARE(cmp_a, cmp_b) \
    (strcmp(cmp_a.name, cmp_b) == 0)

#define STRUCTURE_FIELD_TYPE  struct DocgenStructureField
#define STRUCTURE_FIELD_HEAP  1

#include "../../docgen.h"

/*
 * A field in a structure.
*/
struct DocgenStructureField {
    char name[DOCGEN_STRUCTURE_FIELD_NAME_LENGTH + 1];
    char type[DOCGEN_STRUCTURE_FIELD_TYPE_LENGTH + 1];
    char description[DOCGEN_STRUCTURE_FIELD_DESCRIPTION_LENGTH + 1];
};

/*
 * Array of structure fields.
*/
struct DocgenStructureFields {
    CARRAY_COUNTER_TYPE length;
    CARRAY_COUNTER_TYPE capacity;
    struct DocgenStructureField *contents;
};

/*
 * A structure in the source code of a program.
*/
struct DocgenStructure {
     char name[DOCGEN_STRUCTURE_NAME_LENGTH + 1];
     char brief[DOCGEN_STRUCTURE_BRIEF_LENGTH + 1];
     struct DocgenStructureFields *fields;
     struct DocgenStructures *nested;
};

/*
 * An array of structures.
*/
struct DocgenStructures {
    CARRAY_COUNTER_TYPE length;
    CARRAY_COUNTER_TYPE capacity;
    struct DocgenStructure *contents;
};

int docgen_get_longest_field(struct DocgenStructures *structures, int depth);
void docgen_extract_structures_free(struct DocgenStructures *structures);
struct DocgenStructures *docgen_extract_structures(struct LibmatchCursor *cursor,
                                                   const char *comment_start,
                                                   const char *comment_end);

#endif
