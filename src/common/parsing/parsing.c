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
 * This file implements common file parsing routines.
*/

#include <ctype.h>

#include "../../docgen.h"

#include "parsing.h"

#define LINE_LENGTH 128

/*
 * This function will read lines from a file location into an
 * array of cstrings. However, it is not exactly that simple,
 * as this function facilitates memory reuse. Specifically,
 * the function allowws for re-use of a previously creaated array
 * of strings, including the existing lines.
 *
 * These kinds of functions are a bit harder to write since we need
 * to account for more than we would if we just made a new array.
 * These things are:
 *  - Are there any lines we can reuse?
 *  - How many lines can we re-use?
 *  - How many are left to reuse?
 *
 * We start by iterating over each existing line in the array. For
 * each existing line in the array, we will reset it, and then we
 * will read characters into that line until the line ending is met,
 * or an EOF is met.
 *
 * If an EOF is met before the for loop completes, that means that
 * the new file is smaller, or equal to the length of the previous
 * file in terms of line count. This leads to an important note we
 * must make about reading a smaller file than the previous one we
 * read. Remember that we want to use all existing lines in the array
 * before we start adding new ones.
 *
 * Reading a smaller file after a larger one is a problem because it
 * can lead to discrepency between the number of lines we actually
 * used, and how many are filled. Reading a smaller file will not use
 * all of the lines, and so the array length will not report an accurate
 * statistic and is likely to lead to bugged code. To counter this, we
 * return the actual number of lines read from the function.
 *
 * After we have read into all the existing lines, we now have to make
 * new lines. We read a character, and for each line ending, we flush
 * the buffer, adding the old line into the array, and then allocate a
 * new cstring for the next line. Do note, however, that this can lead
 * to a memory leak if done incorrectly. Consider the following file.
 * Line endings are represented as '$'.
 *
 * foo$
 * bar$
 * baz$
 * $
 *
 * The first 3 lines get allocated as normal, but the last line, an empty
 * line, can have some problems. The previous implementation of this
 * function before the leak was fixed only flushed excess characters if
 * there was any to flush. The problem is, that for each new line read,
 * a new cstring is allocated whether or not there is actually any text
 * to read on that next line.
 *
 * In this scenario, the line is empty except for the line ending character
 * itself. Once that is read, a new line will be made after the empty line.
 * This can lead to a memory leak because of the immediate EOF after the last
 * line ending character, as since the last line ending allocated an extra
 * cstring in anticpation for any other lines, the cstring used for the anticipated
 * extra line is never filled with anything, nor is the buffer used to put
 * less pressure on malloc, because, well, there are no more lines to read.
 *
 * Since the temporary buffer is never filled, the code after the while loop
 * to flush any excess text never flushed by a newline character does not add
 * the extra cstring to the array, and thus a memory leak occurs. We fixed this
 * by always appending the newly allocated line, although a better approach would
 * be to only allocate a new cstring when there is text to read perhaps.
 * 
*/
int common_parse_readlines(struct CStrings *array, FILE *location) {
    int index = 0;
    int character = -1;
    int actual_lines = 0;
    int buffer_length = 0;
    struct CString new_line;
    char line_buffer[LINE_LENGTH + 1] = "";

    VERIFY_CARRAY(array);
    LIBERROR_IS_NULL(location);
    LIBERROR_INIT(new_line);
    LIBERROR_INIT(line_buffer);

    /* For each existing line in the array, put a new line into it. In
     * the case where we have a new array, this for loop will not run,
     * as the for index is equal to the length of the array (0) */
    for(index = 0; index < carray_length(array); index++) {
        struct CString *line = array->contents + index;

        LIBERROR_OUT_OF_BOUNDS(index, carray_length(array));
        LIBERROR_IS_NULL(line);

        cstring_reset(line);

        /* We only want to read a line */
        while((character = fgetc(location)) != '\n') {
            /* End of file is met before the line ending. Prematurely
             * flush the buffer and return to caller. */
            if(character == EOF) {
                line_buffer[buffer_length] = '\0';
                cstring_concats(line, line_buffer);
                buffer_length = 0;

                /* Despite there being no final line which would
                 * lead to the while loop ending and thus incrementing
                 * the line countter, we still read text, so I will
                 * consider this a line of its own. */
                return actual_lines + 1;
            }

            LIBERROR_OUT_OF_BOUNDS(buffer_length, LINE_LENGTH);

            line_buffer[buffer_length] = character;
            buffer_length++;

            /* Line buffer is not full. Do not flush it yet. */
            if(buffer_length < LINE_LENGTH)
                continue;

            line_buffer[buffer_length] = '\0';
            cstring_concats(line, line_buffer);
            buffer_length = 0;
        }

        actual_lines++;
    }

    new_line = cstring_init("");

    /* Read excess lines of the file, extending the length of the array */
    while((character = fgetc(location)) != EOF) {
        /* End of this line. Flush the buffer and make a new line. */
        if(character == '\n') {
            line_buffer[buffer_length] = '\0';
            cstring_concats(&new_line, line_buffer);
            buffer_length = 0;
            actual_lines++;

            /* Add the old string, and make a new one. */
            carray_append(array, new_line, CSTRING);
            new_line = cstring_init("");

            continue;
        } 

        /* Fill up the buffer */
        LIBERROR_OUT_OF_BOUNDS(buffer_length, LINE_LENGTH);

        line_buffer[buffer_length] = character;
        buffer_length++;

        /* Line buffer is not full. Do not flush it yet. */
        if(buffer_length < LINE_LENGTH)
            continue;

        line_buffer[buffer_length] = '\0';
        cstring_concats(&new_line, line_buffer);
        buffer_length = 0;
    }

    /* This is kind of a hacky solution. The problem is that there will always be
     * 1 more extra line allocated. Originally, this  was causing an issue with
     * through a memory leak, so we added the extra line to the array so that it
     * would be freed along with the other ones. However, the error checking for
     * indexing the string would cause problems, as its an empty string, so indexing
     * for example 0 would be out of bounds.
     *
     * To fix this, we only append the extra string if it had text in it, which
     * would be when there was was a line without any line ending character at the
     * end of it, and in the case where the length was zero, we release it.
     * 
     * This function is, of course, not entirely optimal. I think we should create
     * a a better solution in the future.
    */
    if(buffer_length > 0) {
        line_buffer[buffer_length] = '\0';
        cstring_concats(&new_line, line_buffer);
        buffer_length = 0;
        actual_lines++;
    }

    /* In situations where the line length is zero, that means that each
     * line of the stdin had text before the end of the line. This means
     * that the extra allocated line should now be discarded. */
    if(new_line.length == 0) {
        cstring_free(new_line);
    } else {
        carray_append(array, new_line, CSTRING);
    }

    return actual_lines;
}

/*
 * Determine whether or noot the line provided has a docgen tag on it.
 * It is determined based off looping through the string, and if an '@'
 * is found, before an opening or closing single quote, then that is the
 * start of a tag.
 *
 * If a '@' is found, we can safely assume that no quote was before it,
 * because if there was a quote, it would have stopped the function
 * before the '@' was found.
 *
 * If we find a quote, we can assume it is not after a '@' because if it
 * was, the function would have been stopped.
 *
 * In other words, these two states are mutually exclusive.
*/
int common_parse_line_has_tag(struct CString line) {
    int index = 0;

    VERIFY_CSTRING(&line);

    for(index = 0; index < line.length; index++) {
        char character = -1;

        LIBERROR_OUT_OF_BOUNDS(index, line.length);
        character = line.contents[index];

        if(character == '\'')
            return 0;

        if(character == '"')
            return 0;

        if(character == '@')
            return 1;
    }

    return 0;
}

int common_parse_get_tag_index(struct CString line) {
    int index = 0;

    VERIFY_CSTRING(&line);

    for(index = 0; index < line.length; index++) {
        char character = -1;

        LIBERROR_OUT_OF_BOUNDS(index, line.length);
        character = line.contents[index];

        if(character == '\'')
            return -1;

        if(character == '"')
            return -1;

        if(character == '@')
            return index;
    }

    return -1;
}

/* This function will read the name of a tag from a line, and write it
 * into the given cstring. The name of the tag is defined as all the text
 * from the first '@' to the first non-alphabetical or underscore character.
*/
struct CString *common_parse_read_tag(struct CString line, struct CString *location) {
    int index = 0;
    const char *at_sign = NULL;

    VERIFY_CSTRING(&line);
    VERIFY_CARRAY(location); 
    LIBERROR_IS_NULL(strchr(line.contents, '@'));

    /* Reset the cstring given to us, to facilitate memory reuse.*/
    cstring_reset(location);
    at_sign = strchr(line.contents, '@');

    /* Since '@' is technically non alphanumeric, and not an underscore, and we know
     * its in the string, add it first. */
    cstring_concats(location, "@");

    /* From each character after the '@', add the character until any
     * non-alphabetical, and non-underscore character is reached, with
     * notable characters being ":", and "\0" */
    for(index = CHAR_OFFSET(line.contents, at_sign) + 1; index < line.length; index++) {
        char character[2] = {0x0, 0x0};

        LIBERROR_OUT_OF_BOUNDS(index, line.length);

        character[0] = line.contents[index]; 

        /* First non-alphabetical or underscore character! */
        if(strchr(CLASS_ALPHA "_", character[0]) == NULL)
            break;

        cstring_concats(location, character);
    }

    return location;
}

/* Display the string converted to uppercase, and translate
 * underscores into spaces. */
void common_parse_upper_string(FILE *location, const char *string, int length) {
    int index = 0;

    LIBERROR_IS_NULL(location);
    LIBERROR_IS_NULL(string);
    LIBERROR_IS_NEGATIVE(length);
    LIBERROR_IS_VALUE(length, 0);

    for(index = 0; index < length; index++) {
        LIBERROR_OUT_OF_BOUNDS(index, length);

        if(string[index] == '_')
            fprintf(location, "%c", ' ');
        else if(isalpha(string[index]) != 0)
            fprintf(location, "%c", toupper(string[index]));
        else
            fprintf(location, "%c", string[index]);
    }
}


/* ====================================
 * # Common backend parsing functions #
 * ===================================
*/
void common_parse_embeds(struct CStrings lines, struct Embeds *array) {
    int in_body = 0;
    int line_index = 0;
    struct Embed embed;

    VERIFY_CARRAY(array);
    VERIFY_CARRAY(&lines);
    LIBERROR_INIT(embed);

    for(line_index = 0; line_index < carray_length(&lines); line_index++) {
        struct CString line; 

        LIBERROR_INIT(line);
        VERIFY_CSTRING(lines.contents + line_index);

        line = lines.contents[line_index];

        /* We only want to parse when we start the embed (the space
         * here is important to distinguish it from "START_EMBED_REQUEST" */
        if(strncmp(line.contents, "START_EMBED ", strlen("START_EMBED ")) != 0) {
            /* Stop parsing the body string (since theres no space after,
             * theres no need for the extra space */
            if(strcmp(line.contents, "END_EMBED") == 0) {
                in_body = 0;
                carray_append(array, embed, EMBED);

                continue; 
            }

            /* If this is 1, we know we passed "START_EMBED ", and we have not
             * yet met "END_EMBED ". */
            if(in_body == 0)
                continue;

            cstring_concats(&embed.body, line.contents);
            cstring_concats(&embed.body, "\n");

            continue;
        }


        /* We will only get here once, which is when "START_EMBED " is found.
         * Every other time when the string is not START_EMBED, it will add
         * text to the body. */
        embed.name = cstring_init("");
        embed.body = cstring_init("");

        /* Get the name and type */
        cstring_concats(&(embed.name), strchr(line.contents, ' ') + 1);
        embed.type = strtoul(lines.contents[line_index + 1].contents, NULL, 10);

        in_body = 1;

        /* Type is the first 'line' after the directive, so we skip it */
        line_index++;
    }
}

void common_parse_embed_requests(struct CStrings lines, struct EmbedRequests *array, int start_index) {
    int line_index = 0;

    VERIFY_CARRAY(array);
    VERIFY_CARRAY(&lines);
    LIBERROR_IS_NEGATIVE(start_index);

    for(line_index = start_index; line_index < carray_length(&lines); line_index++) {
        struct CString line; 
        struct EmbedRequest embed_request;

        LIBERROR_INIT(line);
        LIBERROR_INIT(embed_request);
        VERIFY_CSTRING(lines.contents + line_index);

        line = lines.contents[line_index];

        /* Stop when we find the end of this group */
        if(strcmp(line.contents, "END_GROUP") == 0)
            break;
        
        if(strncmp(line.contents, "START_EMBED_REQUEST", strlen("START_EMBED_REQUEST")) != 0)
            continue;

        LIBERROR_OUT_OF_BOUNDS(line_index + 1, carray_length(&lines));

        /* Add a new embed request */
        embed_request.name = cstring_init("");
        cstring_concats(&(embed_request.name), strchr(lines.contents[line_index].contents, ' ') + 1);

        /* Are comments allowed? */
        embed_request.allow_comment = strtoul(lines.contents[line_index + 1].contents, NULL, 10);

        carray_append(array, embed_request, EMBED_REQUEST);
    }
}

void common_parse_prepends(struct CStrings lines, struct Sections *array, int start_index) {
    int in_body = 0;
    int line_index = 0;
    struct Section *section = NULL;

    VERIFY_CARRAY(array);
    VERIFY_CARRAY(&lines);
    LIBERROR_IS_NEGATIVE(start_index);
    LIBERROR_INIT(section);

    /* First things first-- let's collect all of the prepends */
    for(line_index = start_index; line_index < carray_length(&lines); line_index++) {
        struct CString line; 

        LIBERROR_INIT(line);
        VERIFY_CSTRING(lines.contents + line_index);

        line = lines.contents[line_index];

        /* Stop when we find the end of this group */
        if(strcmp(line.contents, "END_GROUP") == 0)
            break;

        /* Start a new section, or use an existing one. */
        if(strncmp(line.contents, "START_PREPEND_TO", strlen("START_PREPEND_TO")) == 0) {
            int exists = 0;
            struct Section search;

            LIBERROR_INIT(search);

            in_body = 1; 

            /* If the section already exists, we should use the existing one. */
            search.name.contents = strchr(line.contents, ' ') + 1;
            exists = carray_find(array, search, exists, SECTION);

            if(exists == -1) {
                struct Section new_section;

                new_section.name = cstring_init(""); 
                new_section.body = cstring_init(""); 

                /* All we can know from this line is the name of the section */
                cstring_concats(&(new_section.name), strchr(line.contents, ' ') + 1);

                carray_append(array, new_section, SECTION);
                section = array->contents + (carray_length(array) - 1);
            } else {
                section = array->contents + exists;
            }

            continue;
        }

        /* Stop when the line is the end of a group */
        if(strcmp(line.contents, "END_PREPEND_TO") == 0) {
            in_body = 0;

            continue;
        }

        if(in_body == 0)
            continue;

        /* Construct the body */
        cstring_concats(&(section->body), line.contents);
        cstring_concats(&(section->body), "\n");
    }
}

void common_parse_sections(struct CStrings lines, struct Sections *array, int start_index) {
    int in_body = 0;
    int line_index = 0;
    struct Section *section = NULL;

    VERIFY_CARRAY(array);
    VERIFY_CARRAY(&lines);
    LIBERROR_IS_NEGATIVE(start_index);
    LIBERROR_INIT(section);

    /* Next, let's collect all of the base sections */
    for(line_index = start_index; line_index < carray_length(&lines); line_index++) {
        struct CString line; 

        LIBERROR_INIT(line);
        VERIFY_CSTRING(lines.contents + line_index);

        line = lines.contents[line_index];

        /* Stop when we find the end of this group */
        if(strcmp(line.contents, "END_GROUP") == 0)
            break;

        /* Start a new section, or use an existing one. */
        if(strncmp(line.contents, "START_SECTION", strlen("START_SECTION")) == 0) {
            int exists = 0;
            struct Section search;

            LIBERROR_INIT(search);

            in_body = 1; 

            /* If the section already exists, we should use the existing one. */
            search.name.contents = strchr(line.contents, ' ') + 1;
            exists = carray_find(array, search, exists, SECTION);

            if(exists == -1) {
                struct Section new_section;

                new_section.name = cstring_init(""); 
                new_section.body = cstring_init(""); 

                /* All we can know from this line is the name of the section */
                cstring_concats(&(new_section.name), strchr(line.contents, ' ') + 1);

                carray_append(array, new_section, SECTION);
                section = array->contents + (carray_length(array) - 1);
            } else {
                section = array->contents + exists;
            }

            continue;
        }

        /* Stop when the line is the end of a group */
        if(strcmp(line.contents, "END_SECTION") == 0) {
            in_body = 0;

            continue;
        }

        if(in_body == 0)
            continue;

        /* Construct the body */
        cstring_concats(&(section->body), line.contents);
        cstring_concats(&(section->body), "\n");
    }
}

void common_parse_appends(struct CStrings lines, struct Sections *array, int start_index) {
    int in_body = 0;
    int line_index = 0;
    struct Section *section = NULL;

    VERIFY_CARRAY(array);
    VERIFY_CARRAY(&lines);
    LIBERROR_IS_NEGATIVE(start_index);
    LIBERROR_INIT(section);

    /* Next, let's collect all of the base sections */
    for(line_index = start_index; line_index < carray_length(&lines); line_index++) {
        struct CString line; 

        LIBERROR_INIT(line);
        VERIFY_CSTRING(lines.contents + line_index);

        line = lines.contents[line_index];

        /* Stop when we find the end of this group */
        if(strcmp(line.contents, "END_GROUP") == 0)
            break;

        /* Start a new section, or use an existing one. */
        if(strncmp(line.contents, "START_APPEND_TO", strlen("START_APPEND_TO")) == 0) {
            int exists = 0;
            struct Section search;

            LIBERROR_INIT(search);

            in_body = 1; 

            /* If the section already exists, we should use the existing one. */
            search.name.contents = strchr(line.contents, ' ') + 1;
            exists = carray_find(array, search, exists, SECTION);

            if(exists == -1) {
                struct Section new_section;

                new_section.name = cstring_init(""); 
                new_section.body = cstring_init(""); 

                /* All we can know from this line is the name of the section */
                cstring_concats(&(new_section.name), strchr(line.contents, ' ') + 1);

                carray_append(array, new_section, SECTION);
                section = array->contents + (carray_length(array) - 1);
            } else {
                section = array->contents + exists;
            }

            continue;
        }

        /* Stop when the line is the end of a group */
        if(strcmp(line.contents, "END_APPEND_TO") == 0) {
            in_body = 0;

            continue;
        }

        if(in_body == 0)
            continue;

        /* Construct the body */
        cstring_concats(&(section->body), line.contents);
        cstring_concats(&(section->body), "\n");
    }
}

void common_parse_references(struct CStrings lines, struct References *array, int start_index) {
    int line_index = 0;

    VERIFY_CARRAY(array);
    VERIFY_CARRAY(&lines);
    LIBERROR_IS_NEGATIVE(start_index);

    for(line_index = start_index; line_index < carray_length(&lines); line_index++) {
        struct CString line; 
        struct Reference reference;

        LIBERROR_INIT(line);
        LIBERROR_INIT(reference);
        VERIFY_CSTRING(lines.contents + line_index);

        line = lines.contents[line_index];

        /* Stop when we find the end of this group */
        if(strcmp(line.contents, "END_GROUP") == 0)
            break;
        
        if(strcmp(line.contents, "START_REFERENCE") != 0)
            continue;

        LIBERROR_OUT_OF_BOUNDS(line_index + 1, carray_length(&lines));

        /* Add a new reference */
        reference.name = cstring_init("");
        reference.category = cstring_init("");

        cstring_concat(&(reference.name), lines.contents[line_index + 1]);
        cstring_concat(&(reference.category), lines.contents[line_index + 2]);

        carray_append(array, reference, REFERENCE);
    }
}

int common_parse_highest_type(struct Embeds array) {
    int embed_index = 0;
    int type = -1;

    for(embed_index = 0; embed_index < carray_length(&array); embed_index++) {
        if(array.contents[embed_index].type < type)
            continue;

        type = array.contents[embed_index].type;
    }

    return type;
}

int common_parse_count_types(struct Embeds array, int type) {
    int count = 0;
    int embed_index = 0;

    LIBERROR_IS_NEGATIVE(type);

    for(embed_index = 0; embed_index < carray_length(&array); embed_index++) {
        if(array.contents[embed_index].type != type)
            continue;

        count++;    
    }

    return count;
}

/*
 * ========================
 * # Formatting functions #
 * ========================
*/
struct CString *common_parse_format_embeds(struct Embeds embeds, struct EmbedRequests requests, struct CString *embed_location) {
    int type_id = 0;
    int embed_index = 0;
    struct Embeds *filtered_embeds = NULL;
    struct Embeds *merged_embeds = NULL;

    filtered_embeds = carray_init(filtered_embeds, EMBED);
    merged_embeds = carray_init(merged_embeds, EMBED);

    /* Fill up the embed bodies with the bodies of all the requested embeds,
     * ordered by the types. */
    for(type_id = 0; type_id < common_parse_highest_type(embeds) + 1; type_id++) {
        /* For each embed, if its of the type we expect, and is requested, add it. */
        for(embed_index = 0; embed_index < carray_length(&embeds); embed_index++) {
            int requested_index = -1;
            struct Embed new_embed;

            /* Not the type we're looking for */
            if(embeds.contents[embed_index].type != type_id)
                continue;

            requested_index = carray_find(&requests, embeds.contents[embed_index].name.contents, requested_index, EMBED_REQUEST);

            /* This embed was not requested. */
            if(requested_index == -1)
                continue;

            /* Duplicate the embed with the comment removed if needed.
             * The initialization of the embed name is TECHNICALLY
             * unnecessary, it just exists so we do not attempt to
             * free NULL. */
            new_embed.name = cstring_init("");
            new_embed.body = cstring_init("");
            new_embed.type = embeds.contents[embed_index].type;

            if(requests.contents[requested_index].allow_comment == 0) {
                cstring_concats(&(new_embed.body), strchr(embeds.contents[embed_index].body.contents, '\n') + 1);
                new_embed.has_comment = 0;
            } else {
                cstring_concat(&(new_embed.body), embeds.contents[embed_index].body);
                new_embed.has_comment = 1;
            }

            carray_append(filtered_embeds, new_embed, EMBED);
        }
    }

    /* Merge the uncommented types, then merge the commented ones */
    for(type_id = 0; type_id < common_parse_highest_type(*filtered_embeds) + 1; type_id++) {
        int commented_embeds = 0;
        struct Embed commented;
        struct Embed uncommented;

        /* Do not try to initialize new bodies for types with no embeds in it */
        if(common_parse_count_types(embeds, type_id) == 0)
            continue;

        LIBERROR_INIT(commented);
        LIBERROR_INIT(uncommented);

        commented.name = cstring_init("");
        commented.body = cstring_init("");
        commented.has_comment = 1;

        uncommented.name = cstring_init("");
        uncommented.body = cstring_init("");
        uncommented.has_comment = 0;

        /* Add the uncommented and commented requested embeds that match this type */
        for(embed_index = 0; embed_index < carray_length(filtered_embeds); embed_index++) {
            if(filtered_embeds->contents[embed_index].type != type_id)
                continue;

            if(filtered_embeds->contents[embed_index].has_comment == 1) {
                /* There was a commented embed before us, so add a new line */
                if(commented_embeds > 0)
                    cstring_concats(&commented.body, "\n");

                cstring_concats(&commented.body, filtered_embeds->contents[embed_index].body.contents);
                commented_embeds++;
            } else if(filtered_embeds->contents[embed_index].has_comment == 0) {
                cstring_concats(&uncommented.body, filtered_embeds->contents[embed_index].body.contents);
            } else {
                fprintf(LIBERROR_STREAM, "Should not get here (%s:%i)\n", __FILE__, __LINE__); 
                abort();
            }
            
            continue;
        }

        if(commented.body.length > 0) {
            carray_append(merged_embeds, commented, EMBED); 
        } else {
            EMBED_FREE(commented); 
        }

        if(uncommented.body.length > 0) {
            carray_append(merged_embeds, uncommented, EMBED); 
        } else {
            EMBED_FREE(uncommented); 
        }
    }

    /* Merge the final array into a single string */
    for(embed_index = 0; embed_index < carray_length(merged_embeds); embed_index++) {
        cstring_concats(embed_location, merged_embeds->contents[embed_index].body.contents);

        if(embed_index == (carray_length(merged_embeds) - 1))
            continue;

        cstring_concats(embed_location, "\n");
    }

    carray_free(merged_embeds, EMBED);
    carray_free(filtered_embeds, EMBED);

    return embed_location;
}

int common_parse_count_lines_between_multilines(struct CStrings lines, int index, const char *multiline) {
    int line_index = 0;
    int counted_lines = 0;

    /* We go past the first opening multiline then search */
    for(line_index = index + 1; line_index < lines.length; line_index++) {
        struct CString line = lines.contents[line_index];

        if(strcmp(strchr(line.contents, '@'), multiline) != 0) {
            counted_lines++;

            continue;
        }

        break;
    }

    return counted_lines;
}
