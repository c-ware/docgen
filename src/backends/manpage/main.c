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
 * Takes in compiled input, and for each page, produce a manual.
 *
 * The compiled input is composed of a collection of pages, which are distinct
 * units of documentation. The documentation is in turn composed of 'sections'
*/

#include "../../docgen.h"

#include "../../common/errors/errors.h"
#include "../../common/parsing/parsing.h"

#include "main.h"

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

        /* Get the name */
        cstring_concats(&(embed.name), strchr(line.contents, ' ') + 1);
        in_body = 1;
    }
}

void common_parse_embed_requests(struct CStrings lines, struct CStrings *array, int start_index) {
    int line_index = 0;

    VERIFY_CARRAY(array);
    VERIFY_CARRAY(&lines);
    LIBERROR_IS_NEGATIVE(start_index);

    for(line_index = start_index; line_index < carray_length(&lines); line_index++) {
        struct CString line; 
        struct CString embed_request;

        LIBERROR_INIT(line);
        LIBERROR_INIT(embed_request);
        VERIFY_CSTRING(lines.contents + line_index);

        line = lines.contents[line_index];

        /* Stop when we find the end of this group */
        if(strcmp(line.contents, "END_GROUP") == 0)
            break;
        
        if(strcmp(line.contents, "START_EMBED_REQUEST") != 0)
            continue;

        LIBERROR_OUT_OF_BOUNDS(line_index + 1, carray_length(&lines));

        /* Add a new embed request */
        embed_request = cstring_init("");
        cstring_concat(&embed_request, lines.contents[line_index + 1]);

        carray_append(array, embed_request, CSTRING);
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

int main(void) {
    int index = 0;
    struct Embeds *embeds = NULL;
    struct CStrings *input_lines = NULL;

    embeds = carray_init(embeds, EMBED);
    input_lines = carray_init(input_lines, CSTRING);
    
    /* Collect the data we need */
    common_parse_embeds(*input_lines, embeds);
    common_parse_readlines(input_lines, stdin);

    /* For each group, parse the data we need and put it into a manual */
    for(index = 0; index < carray_length(input_lines); index++) {
        int index_ = 0;
        struct CString line;
        struct Sections *sections = NULL;
        struct CStrings *embed_requests = NULL;

        LIBERROR_OUT_OF_BOUNDS(index, carray_length(input_lines));
        VERIFY_CSTRING(input_lines->contents + index);

        line = input_lines->contents[index];

        if(strcmp(line.contents, "START_GROUP") != 0)
            continue; 

        sections = carray_init(sections, SECTION);
        embed_requests = carray_init(embed_requests, CSTRING);

        /* Extract this group's data */
        common_parse_prepends(*input_lines, sections, index);
        common_parse_sections(*input_lines, sections, index);
        common_parse_appends(*input_lines, sections, index);
        common_parse_embed_requests(*input_lines, embed_requests, index);

        /* What are the sections we have? */
        for(index_ = 0; index_ < carray_length(sections); index_++) {
            printf("%s:\n%s\n", sections->contents[index_].name.contents, sections->contents[index_].body.contents);
        }

        printf("%s", "Requested embeds:\n");
        for(index_ = 0; index_ < carray_length(embed_requests); index_++) {
            printf("\t%s\n",  embed_requests->contents[index_].contents);
        }

        printf("%s", "Next group...\n");

        carray_free(sections, SECTION);
        carray_free(embed_requests, CSTRING);
    }

    carray_free(embeds, EMBED);
    carray_free(input_lines, CSTRING);

    return 0;
}
