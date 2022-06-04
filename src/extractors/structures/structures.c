/*
 * This file is used to extract structures from a buffer.
*/

#include "structures.h"

int docgen_get_longest_field(struct DocgenStructures *structures, int depth) {
    int index = 0;
    int longest = 0;

    /* Get the longest field out of all of these structures */
    for(index = 0; index < carray_length(structures); index++) {
        int field_index = 0;
        int sub_longest = 0;
        struct DocgenStructure structure = structures->contents[index];

        /* Calculate the longest field */
        for(field_index = 0; field_index < carray_length(structure.fields); field_index++) {
            int field_length = 0;
            struct DocgenStructureField field = structure.fields->contents[field_index];

            field_length = (DOCGEN_INDENTATION * (depth + 1))
                           + strlen(field.type) + strlen(field.name) + 1 + 1;
            
            /* Update longest field */
            if(field_length > longest)
                longest = field_length;
        }

        /* Are there longer fields in nested structures? */
        sub_longest = docgen_get_longest_field(structure.nested, depth + 1);

        if(sub_longest > longest)
            longest = sub_longest;
    }

    return longest;
}

struct DocgenStructure docgen_extract_parse_structure(struct LibmatchCursor *cursor,
                                                      const char *comment_start,
                                                      const char *comment_end) {
    struct DocgenStructure new_structure;

    memset(&new_structure, 0, sizeof(struct DocgenStructure));

    /* Initialize fields */
    new_structure.fields = carray_init(new_structure.fields, STRUCTURE_FIELD);
    new_structure.nested = carray_init(new_structure.nested, STRUCTURE);

    while(1) {
        struct DocgenTagName tag_name;
        struct DocgenTag new_tag = docgen_tag_next(cursor, comment_start, comment_end);

        memset(&tag_name, 0, sizeof(struct DocgenTagName));

        /* Handle errors from the tag parser */
        tag_error(&new_tag, cursor);

        /* Extract the tag name */
        tag_name = docgen_tag_name(new_tag);

        /* Handle errors from the tag name extraction */
        switch(tag_name.status) {
            case DOCGEN_TAG_STATUS_FULL:
                fprintf(stderr, "docgen: name of tag on line %i is too long. maximum size of %i\n",
                        cursor->line, DOCGEN_TAG_NAME_LENGTH - 1);
                exit(EXIT_FAILURE);
        }

        if(strcmp(tag_name.name, "name") == 0)
            docgen_extract_field_line("name", DOCGEN_STRUCTURE_NAME_LENGTH, cursor->line,
                                      new_tag.line, new_structure.name);

        else if(strcmp(tag_name.name, "brief") == 0)
            docgen_extract_field_line("brief", DOCGEN_STRUCTURE_BRIEF_LENGTH, cursor->line,
                                      new_tag.line, new_structure.brief);

        else if(strcmp(tag_name.name, "field") == 0) {
            struct DocgenTag type_tag;
            struct DocgenTagName type_tag_name;
            struct DocgenStructureField new_field;

            memset(&type_tag, 0, sizeof(struct DocgenTag));
            memset(&type_tag_name, 0, sizeof(struct DocgenTagName));
            memset(&new_field, 0, sizeof(struct DocgenStructureField));

            docgen_extract_field_line_arg("field", new_tag.line, DOCGEN_STRUCTURE_NAME_LENGTH,
                                          new_field.name, DOCGEN_STRUCTURE_BRIEF_LENGTH,
                                          new_field.description, cursor->line);
            type_tag = docgen_tag_next(cursor, comment_start, comment_end);

            /* Next line must be a type-- cannot even be emppty. */
            next_tag_error(&type_tag, cursor, "field", "type");

            /* Extract the tag name */
            tag_name = docgen_tag_name(type_tag);

            /* Handle errors from the tag name extraction */
            switch(tag_name.status) {
                case DOCGEN_TAG_STATUS_FULL:
                    fprintf(stderr, "docgen: name of tag on line %i is too long. maximum size of %i\n",
                            cursor->line, DOCGEN_TAG_NAME_LENGTH - 1);
                    exit(EXIT_FAILURE);
            }

            /* Parse the type */
            docgen_extract_field_line("type", DOCGEN_STRUCTURE_FIELD_TYPE_LENGTH, cursor->line, type_tag.line, new_field.type);
            carray_append(new_structure.fields, new_field, STRUCTURE_FIELD);

            /* Continue-- to reset new_tag.line */
            continue;
        }

        else if(strcmp(tag_name.name, "struct_start") == 0) {
            struct DocgenStructure nested_structure;

            memset(&nested_structure, 0, sizeof(struct DocgenStructure));
            nested_structure = docgen_extract_parse_structure(cursor, comment_start,
                                                              comment_end);

            carray_append(new_structure.nested, nested_structure, STRUCTURE);
        } else {
            fprintf(stderr, "docgen: unknown tag '%s' in structure extractor on line %i\n",
                    tag_name.name, cursor->line);
            exit(EXIT_FAILURE);
        }
    }

break_loop:    

    return new_structure;
}

struct DocgenStructures *docgen_extract_structures(struct LibmatchCursor *cursor,
                                                   const char *comment_start,
                                                   const char *comment_end) {
    struct DocgenStructures *structures = NULL;
    struct LibmatchCursor new_cursor = libmatch_cursor_init(cursor->buffer,
                                                            cursor->length);

    structures = carray_init(structures, STRUCTURE);

    while(new_cursor.cursor != new_cursor.length) {
        struct DocgenStructure structure;

        memset(&structure, 0, sizeof(struct DocgenStructure));

        if(libmatch_string_expect(&new_cursor, comment_start) == 0)
            continue;

        if(docgen_comment_is_type(&new_cursor, comment_end, comment_start, "structure") == 0)
            continue;

        /* Parse and append the structure */
        structure = docgen_extract_parse_structure(&new_cursor, comment_start, comment_end);
        carray_append(structures, structure, STRUCTURE);
    }

    return structures;
}

void docgen_extract_structures_free(struct DocgenStructures *structures) {
    int index = 0;

    liberror_is_null(docgen_extract_structures_free, structures);

    /* Loop through all structures */
    for(index = 0; index < carray_length(structures); index++) {
        struct DocgenStructure structure = structures->contents[index];

        /* Release all fields of this structure */
        free(structure.fields->contents);
        free(structure.fields);

        /* Repeat on child structures */
        if(structure.nested != NULL) {
            docgen_extract_structures_free(structure.nested);
        }
    }

    free(structures->contents);
    free(structures);
}
