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
 * This file contains functions for the setup and initialization of the
 * program's argument parser.
*/

#include "docgen.h"

/* Retrieve an option's argument */
#define docgen_get_option_argument(longf, shortf, location)                 \
do {                                                                        \
    if((index = libarg_find_option(longf)) != LIBARG_UNKNOWN_OPTION)        \
        location = argv[index + 1];                                         \
    else if((index = libarg_find_option(shortf)) != LIBARG_UNKNOWN_OPTION)  \
        location = argv[index + 1];                                         \
} while(0)

/* Generate option classification boilerplate */
#define docgen_option(longf, shortf, parameters)                    \
    if(strcmp(option, longf) == 0 || strcmp(option, shortf) == 0)   \
        return parameters

void main_load_defaults(struct DocgenArguments *arguments,
                        struct LibargCallbacks *callbacks) {

    arguments->category = NULL;
    arguments->source = NULL;
    arguments->format = "manpage";
    arguments->section = "Section";
    arguments->title = "Title";
    arguments->date = "Date";
    arguments->language = "c";

    callbacks->error = main_error;
    callbacks->parameters = main_parameters;
    callbacks->is_option = libarg_is_unix_option;

    arguments->inclusions = carray_init(arguments->inclusions, INCLUDE);
}

void main_error_check(int argc, char **argv, struct LibargCallbacks libarg_callbacks) {
    libarg_options_check();

    /* Display help */
    if(libarg_find_option("--help") != LIBARG_UNKNOWN_OPTION ||
       libarg_find_option("-h") != LIBARG_UNKNOWN_OPTION)
        main_error(NULL, LIBARG_ERR_HELP, 0, 0);

    if(libarg_count_positionals() != 2) 
        main_error(NULL, LIBARG_ERR_ARG_COUNT, 2, libarg_count_positionals());
}

void main_error(const char *option, int type, int expected, int got) {
    liberror_is_negative(main_error, type);
    liberror_is_negative(main_error, expected);
    liberror_is_negative(main_error, got);

    /* Generate error messages for this type of argument */
    switch(type) {
        case LIBARG_ERR_UNKNOWN_OPTION:
            liberror_is_null(main_error, option); 
            fprintf(stderr, "docgen: unrecognized option '%s'\n", option);
            fprintf(stderr, "%s", "Try 'docgen --help' for more information"
                            "\n");
            break;
        case LIBARG_ERR_ARG_COUNT:
            fprintf(stderr, "docgen: expected %i arguments, got %i\n",
                    expected, got);
            fprintf(stderr, "%s", "Try 'docgen --help' for more information"
                            "\n");
            break;
        case LIBARG_ERR_PARAM_COUNT:
            liberror_is_null(main_error, option); 
            fprintf(stderr, "docgen: option '%s' expects %i parameters, got "
                            "%i\n", option, expected, got);
            fprintf(stderr, "%s", "Try 'docgen --help' for more information"
                            "\n");
            break;
        case LIBARG_ERR_HELP:
            fprintf(stderr, "%s", "Usage: docgen CATEGORY FILE [ --help | -h ] [ --format NAME | -f NAME ]\n");
            fprintf(stderr, "%s", "                            [ --section SECTION | -s SECTION ] [ --title TITLE | -t TITLE ]\n");
            fprintf(stderr, "%s", "                            [ --date DATE | -d DATE ] [ --include INCLUDE | -l INCLUDE ]\n");
            fprintf(stderr, "%s", "                            [ --isystem INCLUDE | -d INCLUDE ] [ --language LANGUAGE | -x LANGUAGE ]\n");
            fprintf(stderr, "%s", "                            [ --md-mono | -m ] \n");
            fprintf(stderr, "%s", "\nArguments:\n");
            fprintf(stderr, "%s", "       category                    the type of documentation to generate\n");
            fprintf(stderr, "%s", "       file                        the file to generate documentation from (can be - for stdin)\n");
            fprintf(stderr, "%s", "\nOptions:\n");
            fprintf(stderr, "%s", "       --help,     -h              show this message\n");
            fprintf(stderr, "%s", "       --format,   -f NAME         select the format to output the manual(s) in\n");
            fprintf(stderr, "%s", "       --include,  -l INCLUDE      specify local files that should be included\n");
            fprintf(stderr, "%s", "       --isystem,  -d INCLUDE      specify system header files that should be included\n");
            fprintf(stderr, "%s", "       --language, -x LANGUAGE     specify the language of the file\n");
            fprintf(stderr, "%s", "\nManpage Options\n");
            fprintf(stderr, "%s", "       --title,    -t TITLE        the title at the top of the manual\n");
            fprintf(stderr, "%s", "       --date,     -d DATE         the current date\n");
            fprintf(stderr, "%s", "       --section,  -s SECTION      the manual section name\n");

            break;
        default:
            liberror_unhandled(main_error);
            break;
    }

    exit(EXIT_FAILURE);
}

void main_extract_inclusions(int argc, char **argv,
                             struct DocgenArguments *arguments) {
    int index = 1;

    liberror_is_null(main_extract_inclusions, argv);
    liberror_is_null(main_extract_inclusions, arguments);
    liberror_is_negative(main_extract_inclusions, argc);

    for(index = 0; index < argc; index++) {
        char *argument = argv[index];
        struct Inclusion new_inclusion;

        memset(&new_inclusion, 0, sizeof(struct Inclusion));

        /* Get inclusions */
        if(strcmp(argument, "--include") == 0 || strcmp(argument, "-l") == 0) {

            new_inclusion.type = DOCGEN_INCLUSION_LOCAL;

            strncat(new_inclusion.path, argv[index + 1], DOCGEN_INCLUSION_PATH_LENGTH);
            carray_append(arguments->inclusions, new_inclusion, INCLUDE);

            continue;
        }

        /* Get system inclusions */
        if(strcmp(argument, "--isystem") == 0 || strcmp(argument, "-i") == 0) {
            new_inclusion.type = DOCGEN_INCLUSION_SYSTEM;

            strncat(new_inclusion.path, argv[index + 1], DOCGEN_INCLUSION_PATH_LENGTH);
            carray_append(arguments->inclusions, new_inclusion, INCLUDE);

            continue;
        }
    }
}

int main_parameters(const char *option) {
    liberror_is_null(main_parameters, option);

    docgen_option("--help", "-h", 0);
    docgen_option("--format", "-f", 1);
    docgen_option("--section", "-s", 1);
    docgen_option("--title", "-t", 1);
    docgen_option("--date", "-d", 1);
    docgen_option("--include", "-l", 1);
    docgen_option("--isystem", "-i", 1);
    docgen_option("--language", "-x", 1);

    return LIBARG_UNKNOWN_OPTION;
}

struct DocgenArguments main_parse(int argc, char **argv) {
    int index = 0;
    struct DocgenArguments arguments;
    struct LibargCallbacks libarg_callbacks;

    liberror_is_null(main_parse, argv);
    liberror_is_negative(main_parse, argc);

    main_load_defaults(&arguments, &libarg_callbacks);
    main_error_check(argc, argv, libarg_callbacks);

    /* Get the various option arguments if they exist */
    docgen_get_option_argument("--format", "-f", arguments.format);
    docgen_get_option_argument("--section", "-s", arguments.section);
    docgen_get_option_argument("--title", "-t", arguments.title);
    docgen_get_option_argument("--date", "-d", arguments.date);
    docgen_get_option_argument("--language", "-x", arguments.language);

    /* Extract inclusions */
    main_extract_inclusions(argc, argv, &arguments);

    /* Extract arguments */
    for(index = 1; index < argc; index++) {
        char *argument = argv[index];

        if(arguments.category == NULL)
            arguments.category = argument;
        else if(arguments.source == NULL)
            arguments.source = argument;
    }
    
    return arguments;
}
