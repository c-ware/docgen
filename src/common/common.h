#ifndef CWARE_DOCGEN_COMMON_H
#define CWARE_DOCGEN_COMMON_H

#define docgen_get_field_length(field, depth) \
    (((depth + 1) * DOCGEN_INDENTATION) + strlen((field).type) + strlen((field).name) + 1 + 1)

struct LibmatchCursor;
struct DocgenArguments;
struct DocgenStructureField;

/* Error handling */
#define tag_error(new_tag, cursor)                                                                         \
    switch((new_tag)->status) {                                                                            \
        case DOCGEN_TAG_STATUS_EMPTY:                                                                      \
            continue;                                                                                      \
        case DOCGEN_TAG_STATUS_DONE:                                                                       \
            goto break_loop;                                                                               \
        case DOCGEN_TAG_STATUS_EOF:                                                                        \
            fprintf(stderr, "docgen: file ended without any closing comment (%s:%i)\n",                    \
                    __FILE__, __LINE__);                                                                   \
                                                                                                           \
            exit(EXIT_FAILURE);                                                                            \
        case DOCGEN_TAG_STATUS_EOC:                                                                        \
            fprintf(stderr, "docgen: comment ends on the same line as a tag (line %i) (%s:%i)\n",          \
                    (cursor)->line, __FILE__, __LINE__);                                                   \
                                                                                                           \
            exit(EXIT_FAILURE);                                                                            \
        case DOCGEN_TAG_STATUS_FULL:                                                                       \
            fprintf(stderr, "docgen: line %i could not fit in line buffer. maximum size of %i (%s:%i)\n",  \
                    (cursor)->line, DOCGEN_TAG_LINE_LENGTH - 1, __FILE__, __LINE__);                       \
            exit(EXIT_FAILURE);                                                                            \
    }                                                                                                      \

#define next_tag_error(type_tag, cursor, first, second)                                                         \
    switch((type_tag)->status) {                                                                                \
        case DOCGEN_TAG_STATUS_EMPTY:                                                                           \
            fprintf(stderr, "docgen: %s and %s tags on line %i cannot have any lines between them (%s:%i)\n",   \
                    (first), (second), (cursor)->line, __FILE__, __LINE__);                                     \
                                                                                                                \
            exit(EXIT_FAILURE);                                                                                 \
        case DOCGEN_TAG_STATUS_DONE:                                                                            \
            fprintf(stderr, "docgen: comment ended at line %i before type could be determined (%s:%i)\n",       \
                    (cursor)->line, __FILE__, __LINE__);                                                        \
                                                                                                                \
            exit(EXIT_FAILURE);                                                                                 \
        case DOCGEN_TAG_STATUS_EOF:                                                                             \
            fprintf(stderr, "docgen: file ended without any closing comment (%s:%i)\n",                         \
                    __FILE__, __LINE__);                                                                        \
                                                                                                                \
            exit(EXIT_FAILURE);                                                                                 \
        case DOCGEN_TAG_STATUS_EOC:                                                                             \
            fprintf(stderr, "docgen: comment ends on the same line as a type tag (line %i) (%s:%i)\n",          \
                    (cursor)->line, __FILE__, __LINE__);                                                        \
                                                                                                                \
            exit(EXIT_FAILURE);                                                                                 \
        case DOCGEN_TAG_STATUS_FULL:                                                                            \
            fprintf(stderr, "docgen: line %i could not fit in line buffer. maximum size of %i (%s:%i)\n",       \
                    (cursor)->line, DOCGEN_TAG_LINE_LENGTH - 1, __FILE__, __LINE__);                            \
            exit(EXIT_FAILURE);                                                                                 \
    }                                                                                                           \

void docgen_extract_type(struct LibmatchCursor *cursor, char *buffer, int length);
void docgen_parse_comment(struct LibmatchCursor *cursor);
int docgen_comment_is_type(struct LibmatchCursor *cursor, const char *comment_start,
                           const char *commend_end, const char *type);
void docgen_extract_block(struct LibmatchCursor *cursor, char *buffer,
                          int length, const char *bound);
const char *docgen_get_comment_start(struct DocgenArguments arguments);
const char *docgen_get_comment_end(struct DocgenArguments arguments);

void docgen_create_file_path(struct DocgenArguments arguments, const char *name,
                             char *buffer, int length);

void docgen_extract_field_line(const char *tag_name, int length, int line,
                               char *read, char *buffer);

void docgen_extract_field_block(const char *tag_name, int length, struct LibmatchCursor *cursor,
                                char *read, char *buffer);

void docgen_extract_field_line_arg(const char *tag_name, char *read, int argument_length,
                                   char *argument_buffer, int description_length,
                                   char *description_buffer, int line);

struct Reference docgen_extract_reference(struct LibmatchCursor *cursor,
                                          struct DocgenTag new_tag);

void docgen_do_padding(struct DocgenStructureField field, int longest, int depth,
                       FILE *location);

#endif
