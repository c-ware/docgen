/*
 * This file contains functions for the setup and initialization of the
 * program's argument parser.
*/

#include "docgen.h"

static const char *help = 
"Usage: docgen CATEGORY FILE [ --help | -h ] [ --format NAME | -f NAME ]\n"
"                            [ --section SECTION | -s SECTION ] [ --title TITLE | -t TITLE ]\n"
"                            [ --date DATE | -d DATE ] [ --include INCLUDE | -l INCLUDE ]\n"
"                            [ --isystem INCLUDE | -d INCLUDE ] [ --language LANGUAGE | -x LANGUAGE ]\n"
"Arguments:\n"
"       category                    the type of documentation to generate\n"
"       file                        the file to generate documentation from (can be - for stdin)\n"
"\n\n"
"Options:\n"
"       --help,     -h              show this message\n"
"       --format,   -f NAME         select the format to output the manual(s) in\n"
"       --section,  -s SECTION      the manual section name\n"
"       --title,    -t TITLE        the title at the top of the manual\n"
"       --date,     -d DATE         the current date\n"
"       --include,  -l INCLUDE      specify local files that should be included\n"
"       --isystem,  -d INCLUDE      specify system header files that should be included\n"
"       --language, -x LANGUAGE     specify the language of the file\n"
""
"";


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
            fprintf(stderr, "%s", help);
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

    arguments->inclusions = carray_init(arguments->inclusions, INCLUDE);

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

int main_enumerate(const char *category) {
    liberror_is_null(main_enumerate, category);

    if(strcmp(category, "functions") == 0)
        return DOCGEN_CATEGORY_FUNCTION;

    if(strcmp(category, "project") == 0)
        return DOCGEN_CATEGORY_PROJECT;

    if(strcmp(category, "category") == 0)
        return DOCGEN_CATEGORY_CATEGORY;

    fprintf(stderr, "docgen: unknown category '%s'\n", category);
    fprintf(stderr, "%s", "Try 'docgen --help' for more information.\n");

    exit(EXIT_FAILURE);

    return -1;
}

int main_enumerate_language(const char *language) {
    liberror_is_null(main_enumerate_language, language);

    if(strcmp(language, "c") == 0)
        return DOCGEN_LANGUAGE_C;

    /* From: single comment mode attempt
    if(strcmp(language, "py") == 0)
        return DOCGEN_LANGUAGE_PY;
    */

    fprintf(stderr, "docgen: unsupported language '%s'\n", language);
    fprintf(stderr, "%s", "Try 'docgen --help' for more information.\n");

    exit(EXIT_FAILURE);
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
