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
    argparse_add_option(&parser, "-s", "--section", 1);
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
struct Section *find_section(struct Sections sections, const char *name) {
    int section_index = 0;

    for(section_index = 0; section_index < carray_length(&sections); section_index++) {
        if(strcmp(sections.contents[section_index].name.contents, name) != 0)
           continue;

       return sections.contents + section_index; 
    }

    return NULL;
}

void add_section(struct Manual *location, struct Sections sections, const char *name) {
    struct Section *named_section = find_section(sections, name);

    if(named_section == NULL)
        return;

    cstring_concats(&(location->body), ".SH ");
    cstring_concat(&(location->body), named_section->name);
    cstring_concats(&(location->body), "\n");
    cstring_concat(&(location->body), named_section->body);
}

void add_embeds(struct Sections sections, struct CString embed_string) {
    struct Section *synopsis_section = find_section(sections, "SYNOPSIS");

    if(synopsis_section == NULL)
        return;

    if(synopsis_section->body.length > 0)
        cstring_concats(&(synopsis_section->body), "\n");

    cstring_concat(&(synopsis_section->body), embed_string);
}

void add_section_see_also(struct Manual *location, struct References references) {
    int section_index = 0;

    /* No references? Do not add this section. */
    if(carray_length(&references) == 0)
        return;

    cstring_concats(&(location->body), ".SH SEE ALSO\n");

    /* Add each section */
    for(section_index = 0; section_index < carray_length(&references); section_index++) {
        cstring_concat(&(location->body), references.contents[section_index].name);
        cstring_concats(&(location->body), "(");
        cstring_concat(&(location->body), references.contents[section_index].category);
        cstring_concats(&(location->body), ")");

        /* Do not add a comma unless there are still references to add */
        if(section_index == (carray_length(&references) - 1))
            continue; 

        cstring_concats(&(location->body), ", ");
    }
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
        struct Manual new_manual;
        struct CString manual_embeds;
        struct Sections *sections = NULL;
        struct References *references = NULL;
        struct EmbedRequests *requests = NULL;
        struct CString line = input_lines.contents[line_index];

        LIBERROR_INIT(new_manual);

        if(strncmp(line.contents, "START_GROUP", strlen("START_GROUP")) != 0)
            continue;

        sections = carray_init(sections, SECTION);
        requests = carray_init(requests, EMBED_REQUEST);
        references = carray_init(references, REFERENCE);
        new_manual.body = cstring_init("");
        new_manual.name = cstring_init("");
        manual_embeds = cstring_init("");

        /* Get the name of the manual (which is right after the START_GROUP directive */
        cstring_concats(&(new_manual.name), strchr(line.contents, ' ') + 1);

        /* Retrieve this group's sections and metadata */
        common_parse_prepends(input_lines, sections, line_index);
        common_parse_sections(input_lines, sections, line_index);
        common_parse_appends(input_lines, sections, line_index);
        common_parse_embed_requests(input_lines, requests, line_index);
        common_parse_references(input_lines, references, line_index);

        /* Generate the synopsis' embed string */
        common_parse_format_embeds(*embeds, *requests, &manual_embeds);

        /* Add an extra line between existing synopsis text, and the embeds, if there is
         * existing text. */
        add_embeds(*sections, manual_embeds);

        /* Manual needs a header */
        cstring_concats(&(new_manual.body), ".TH \"");
        cstring_concat(&(new_manual.body), new_manual.name);
        cstring_concats(&(new_manual.body), "\" \"");
        cstring_concats(&(new_manual.body), arguments.section);
        cstring_concats(&(new_manual.body), "\" \"");
        cstring_concats(&(new_manual.body), arguments.date);
        cstring_concats(&(new_manual.body), "\" \"");
        cstring_concats(&(new_manual.body), "\" \"");
        cstring_concats(&(new_manual.body), arguments.title);
        cstring_concats(&(new_manual.body), "\"\n");

        /* Add the sections to the manual string */
        add_section(&new_manual, *sections, "NAME");
        add_section(&new_manual, *sections, "SYNOPSIS");
        add_section(&new_manual, *sections, "DESCRIPTION");
        add_section(&new_manual, *sections, "NOTES");
        add_section(&new_manual, *sections, "EXAMPLES");

        /* SEE ALSO is something we need to construct manually */
        add_section_see_also(&new_manual, *references);

        /* Add the final manual */
        carray_append(manuals, new_manual, MANUAL);

        carray_free(sections, SECTION);
        carray_free(requests, EMBED_REQUEST);
        carray_free(references, REFERENCE);
        cstring_free(manual_embeds);
    }

    carray_free(embeds, EMBED);

    return manuals;
}

int main(int argc, char **argv) {
    int manual_index = 0;
    struct Manuals *manuals = NULL;
    struct CStrings *input_lines = NULL;
    struct CString manual_path = cstring_init("");
    struct ProgramArguments arguments = parse_arguments(argc, argv);

    input_lines = carray_init(input_lines, CSTRING);
    common_parse_readlines(input_lines, stdin);
    manuals = build_manuals(*input_lines, arguments);

    /* Write each manual to its intended location */
    for(manual_index = 0; manual_index < carray_length(manuals); manual_index++) {
        FILE *manual_file = NULL;
        struct Manual manual = manuals->contents[manual_index];

        cstring_reset(&manual_path);

        /* Ceate the path for the manual */
        cstring_concats(&manual_path, "doc/");
        cstring_concat(&manual_path, manual.name);
        cstring_concats(&manual_path, ".");
        cstring_concats(&manual_path, arguments.section);

        manual_file = fopen(manual_path.contents, "w+");
        LIBERROR_FILE_OPEN_FAILURE(manual_file, manual_path.contents);

        /* Write the manual, translating each 0x10 (linefeed) into
         * `\n.br\n` */
        translate_newlines(manual_file, manual.body);

        fclose(manual_file);
    }

    carray_free(input_lines, CSTRING);
    carray_free(manuals, MANUAL);
    cstring_free(manual_path);

    return 0;    
}
