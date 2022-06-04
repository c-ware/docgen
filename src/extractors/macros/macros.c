/*
 * This file is used to extract macros from a buffer.
*/

#include "macros.h"

struct DocgenMacro docgen_extract_parse_macro(struct LibmatchCursor *cursor,
                                              const char *comment_end) {
    struct DocgenMacro new_macro;

    memset(&new_macro, 0, sizeof(struct DocgenMacro));

    while(cursor->cursor != cursor->length) {
        int tag = 0;

        /* End of comment */
        if(libmatch_string_expect(cursor, comment_end) == 1)
            break;

        /* Skip lines without tags */
        if(libmatch_cond_before(cursor, '@', "\n") == 0) {
            libmatch_next_line(cursor);

            continue;
        }

        /* Get the tag */
        libmatch_until(cursor, "@");
        tag = libmatch_strings_expect(cursor, "name: ", "value: ", "brief: ",
                                      "setting: ", NULL);

        /* Report unknown tags */
        if(tag == -1) {
            fprintf(stderr, "docgen: unknown tag on line %i\n", cursor->line + 1);
            abort();
        }

        switch(tag) {
            case 0: /* Parse a name tag */
                libmatch_read_until(cursor, new_macro.name, DOCGEN_MACRO_NAME_LENGTH, "\n");
                break;
            case 1: /* Parse a value tag */
                libmatch_read_until(cursor, new_macro.value, DOCGEN_MACRO_VALUE_LENGTH, "\n");
                break;
            case 2: /* Parse a brief tag */
                libmatch_read_until(cursor, new_macro.brief, DOCGEN_MACRO_BRIEF_LENGTH, "\n");
                break;
            case 3: /* Parse a settings tag */
                tag = libmatch_strings_expect(cursor, "ifndef", NULL);

                if(tag == -1) {
                    fprintf(stderr, "docgen: unknown setting on line %i\n", cursor->line + 1);
                    abort();
                }

                new_macro.ifndef = 1;

                break;
        }
    }

    return new_macro;
}

struct DocgenMacros *docgen_extract_macros(struct LibmatchCursor *cursor,
                                           const char *comment_start,
                                           const char *comment_end) {
    struct DocgenMacros *macros = NULL;
    struct LibmatchCursor new_cursor = libmatch_cursor_init(cursor->buffer,
                                                            cursor->length);

    macros = carray_init(macros, MACRO);

    while(new_cursor.cursor != new_cursor.length) {
        struct DocgenMacro macro;

        memset(&macro, 0, sizeof(struct DocgenMacro));

        if(libmatch_string_expect(&new_cursor, comment_start) == 0)
            continue;


        if(docgen_comment_is_type(&new_cursor, comment_start, comment_end, "constant") == 0)
            continue;

        /* Parse and append the macro */
        macro = docgen_extract_parse_macro(&new_cursor, comment_end);
        carray_append(macros, macro, MACRO);
    }

    return macros;
}

void docgen_extract_macros_free(struct DocgenMacros *macros) {
    free(macros->contents);
    free(macros);
}
