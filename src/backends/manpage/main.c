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

static const char *help_message =
    "docgen-backend-manpage [ --section SECTION | -s SECTION ]\n"
    "                       [ --title TITLE | -t TITLE ]\n"
    "                       [ --date DATE | -d DATE ]\n"
    "Generate manual pages from compiled input.\n"
    "\n"
    "Optional arguments:\n"
    "   --section, -s SECTION       the section of the manual page. defaults to 1\n"
    "   --title, -t TITLE           the title (top center text) of the manual page. defaults to \"Manual\"\n"
    "   --date, -d DATE             the date the manual was last modified. defaults to an empty string\n"
    "";

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
 * =====================
 * # Argument handling #
 * =====================
*/
struct ProgramArguments parse_arguments(int argc, char **argv) {
    struct ProgramArguments arguments = {"1", "Manual", ""};
    struct ArgparseParser parser = argparse_init("docgen-backend-manapage", argc, argv);

    /* These are the options we want to accept */
    argparse_add_option(&parser, "-c", "--category", 1);
    argparse_add_option(&parser, "-t", "--title", 1);
    argparse_add_option(&parser, "-d", "--date", 1);

    /* Display the help message */
    if(argparse_option_exists(parser, "-h") != 0 || argparse_option_exists(parser, "--help") != 0) {
        fprintf(LIBERROR_STREAM, "%s", help_message); 

        exit(1);
    }

    argparse_error(parser);

    /* Extract the options arguments (they already have assigned defaults) */
    if(argparse_option_exists(parser, "-c") != 0)
        arguments.section = argparse_get_option_parameter(parser, "-c", 0);
    else if(argparse_option_exists(parser, "--category") != 0)
        arguments.section = argparse_get_option_parameter(parser, "--category", 0);

    if(argparse_option_exists(parser, "-t") != 0)
        arguments.title = argparse_get_option_parameter(parser, "-t", 0);
    else if(argparse_option_exists(parser, "--title") != 0)
        arguments.title = argparse_get_option_parameter(parser, "--title", 0);

    if(argparse_option_exists(parser, "-d") != 0)
        arguments.date = argparse_get_option_parameter(parser, "-d", 0);
    else if(argparse_option_exists(parser, "--date") != 0)
        arguments.date = argparse_get_option_parameter(parser, "--date", 0);

    argparse_free(parser);

    return arguments;
}

/*
 * ===================
 * #  Main function  #
 * ===================
*/
int _main(int argc, char **argv) {
    int index = 0;
    struct Embeds *embeds = NULL;
    struct CStrings *input_lines = NULL;
    const char *category = NULL;
    const char *title = NULL;
    const char *date = NULL;

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

        printf("%s\n", output_file_path);

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
        fprintf(output_file, ".TH \"%s\" \"%s\" \"%s\" \"\" \"%s\"\n", strchr(line.contents, ' ') + 1, category, date, title);

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
    }

    carray_free(embeds, EMBED);
    carray_free(input_lines, CSTRING);

    return 0;
}

struct Manuals *build_manuals(struct CStrings input_lines, struct ProgramArguments arguments) {
    int line_index = 0;
    struct Embeds *embeds = NULL;
    struct Manuals *manuals = NULL;

    embeds = carray_init(embeds, EMBED);
    manuals = carray_init(manuals, MANUAL);

    common_parse_embeds(input_lines, embeds);

    /* Generate a manual for each START_GROUP found */
    for(line_index = 0; line_index < carray_length(&input_lines); line_index++) {
        int section_index = 0;
        struct Sections *sections = NULL;
        struct CString line = input_lines.contents[line_index];

        if(strncmp(line.contents, "START_GROUP", strlen("START_GROUP")) != 0)
            continue;

        sections = carray_init(sections, SECTION);

        /* Retrieve this group's sections */
        common_parse_prepends(input_lines, sections, line_index);
        common_parse_sections(input_lines, sections, line_index);
        common_parse_appends(input_lines, sections, line_index);

        for(section_index = 0; section_index < carray_length(sections); section_index++) {
            printf("Name: '%s'\n", sections->contents[section_index].name.contents); 
            printf("Body: '%s'\n", sections->contents[section_index].body.contents); 
        }
    }

    carray_free(embeds, EMBED);

    return manuals;
}

int main(int argc, char **argv) {
    struct Manuals *manuals = NULL;
    struct CStrings *input_lines = NULL;
    struct ProgramArguments arguments = parse_arguments(argc, argv);

    input_lines = carray_init(input_lines, CSTRING);
    common_parse_readlines(input_lines, stdin);
    manuals = build_manuals(*input_lines, arguments);

    carray_free(input_lines, CSTRING);
    carray_free(manuals, MANUAL);

    return 0;    
}
