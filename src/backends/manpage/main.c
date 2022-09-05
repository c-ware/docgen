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

/*
 * ==========================
 * #   Parsing operations   #
 * ==========================
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

/*
 * =====================
 * # Manual Formatters #
 * =====================
*/
void translate_newlines(FILE *location, struct CString string) {
    int character = 0;

    for(character = 0; character < string.length; character++) {
        if(string.contents[character] == '\n') {
            fprintf(location, "%s", "\n.br\n");
        } else {
            fprintf(location, "%c", string.contents[character]);
        }
    }
}

/*
 * ===================
 * #  Main function  #
 * ===================
*/

int main(int argc, char **argv) {
    int index = 0;
    struct Embeds *embeds = NULL;
    struct CStrings *input_lines = NULL;
    struct ArgparseParser parser = argparse_init("docgen-backend-manapage", argc, argv);
    const char *category = NULL;
    const char *title = NULL;
    const char *date = NULL;

    argparse_add_option(&parser, "-c", "--category", 1);
    argparse_add_option(&parser, "-t", "--title", 1);
    argparse_add_option(&parser, "-d", "--date", 1);
    argparse_error(parser);

    if(argparse_option_exists(parser, "-c") != 0)
        category = argparse_get_option_parameter(parser, "-c", 0);
    else if(argparse_option_exists(parser, "--category") != 0)
        category = argparse_get_option_parameter(parser, "--category", 0);
    else
        category = "1";

    if(argparse_option_exists(parser, "-t") != 0)
        title = argparse_get_option_parameter(parser, "-t", 0);
    else if(argparse_option_exists(parser, "--title") != 0)
        title = argparse_get_option_parameter(parser, "--title", 0);
    else
        title = "";

    if(argparse_option_exists(parser, "-d") != 0)
        date = argparse_get_option_parameter(parser, "-d", 0);
    else if(argparse_option_exists(parser, "--date") != 0)
        date = argparse_get_option_parameter(parser, "--date", 0);
    else
        date = "";

    embeds = carray_init(embeds, EMBED);
    input_lines = carray_init(input_lines, CSTRING);
  
    /* Collect the data we need */
    common_parse_readlines(input_lines, stdin);
    common_parse_embeds(*input_lines, embeds);

    /* For each group, parse the data we need and put it into a manual */
    for(index = 0; index < carray_length(input_lines); index++) {
        int index_ = 0;
        struct CString line;
        struct CString synopsis_embed;
        struct Sections *sections = NULL;
        struct References *references = NULL;
        struct EmbedRequests *embed_requests = NULL;
        char output_file_path[OUTPUT_FILE_PATH_LENGTH + 1];
        FILE *output_file = NULL;

        LIBERROR_OUT_OF_BOUNDS(index, carray_length(input_lines));
        VERIFY_CSTRING(input_lines->contents + index);

        line = input_lines->contents[index];

        if(strncmp(line.contents, "START_GROUP", strlen("START_GROUP")) != 0)
            continue; 

        /* Open the output file, but first make sure we can hold a path large
         * enough. */
        if((strlen("doc/") + strlen(strchr(line.contents, ' ') + 1) + strlen(".") + strlen(category)) >= OUTPUT_FILE_PATH_LENGTH) {
            fprintf(LIBERROR_STREAM, "%s", "docgen-backend-manpage: output file path is too long." );
            exit(1);
        }

        sprintf(output_file_path, "doc/%s.%s", strchr(line.contents, ' ') + 1, category);
        output_file = fopen(output_file_path, "w+");

        LIBERROR_IS_NULL(output_file);

        synopsis_embed = cstring_init("");
        sections = carray_init(sections, SECTION);
        embed_requests = carray_init(embed_requests, EMBED_REQUEST);
        references = carray_init(references, REFERENCE);

        /* Extract this group's data */
        common_parse_prepends(*input_lines, sections, index);
        common_parse_sections(*input_lines, sections, index);
        common_parse_appends(*input_lines, sections, index);
        common_parse_embed_requests(*input_lines, embed_requests, index);
        common_parse_format_embeds(*embeds, *embed_requests, &synopsis_embed);
        common_parse_references(*input_lines, references, index);

        /* Header */
        fprintf(output_file, ".TH %s %s \"%s\" \"\" %s\n", strchr(line.contents, ' ') + 1, category, date, title);

        /* Get the name */
        for(index_ = 0; index_ < carray_length(sections); index_++) {
            struct Section section = sections->contents[index_];

            if(strcmp(section.name.contents, "NAME") != 0)
                continue;

            fprintf(output_file, ".SH %s\n.br\n", section.name.contents);
            translate_newlines(output_file, section.body);
        }

        /* Get the synopsis */
        for(index_ = 0; index_ < carray_length(sections); index_++) {
            struct Section section = sections->contents[index_];

            if(strcmp(section.name.contents, "SYNOPSIS") != 0)
                continue;

            fprintf(output_file, "%s", ".SH SYNOPSIS\n.br\n");
            
            /* If the body already exists (i.e there is stuff before the
             * embeds), make a new line for the embeds if there are any */
            if(section.body.length > 0 && synopsis_embed.length > 0) {
                translate_newlines(output_file, section.body);
                fprintf(output_file, "%s", "\n.br\n");
            }

            translate_newlines(output_file, synopsis_embed);
        }

        /* Get the other sections */
        for(index_ = 0; index_ < carray_length(sections); index_++) {
            struct Section section = sections->contents[index_];

            if(strcmp(section.name.contents, "NAME") == 0 || strcmp(section.name.contents, "SYNOPSIS") == 0)
                continue;

            fprintf(output_file, ".SH %s\n.br\n", section.name.contents);
            translate_newlines(output_file, section.body);
        }


        /* Dump the see-also if there is any. */
        if(carray_length(references) > 0) {
            fprintf(output_file, ".SH SEE ALSO\n.br\n"); 

            for(index_ = 0; index_ < carray_length(references); index_++) {
                fprintf(output_file, "%s(%s)", references->contents[index_].name.contents, references->contents[index_].category.contents);

                if(index_ == (carray_length(references) - 1))
                    continue; 

                fprintf(output_file, "%s", ", ");
            }

            fprintf(output_file, "%c", '\n');
        }

        cstring_free(synopsis_embed);
        carray_free(sections, SECTION);
        carray_free(embed_requests, EMBED_REQUEST);

        fclose(output_file);

        break;
    }

    carray_free(embeds, EMBED);
    carray_free(input_lines, CSTRING);
    argparse_free(parser);

    return 0;
}
