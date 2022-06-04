#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#include "libarg.h"

int libarg_is_unix_option(const char *option) {
   int last_index = -1;

   /* Minimum length of 2 */
   if(strlen(option) < 2)
       return 0;

   if(option[0] == '-') {
       last_index = 0;

       if(option[1] == '-')
           last_index = 1;
   } else {
       return 0;
   }

   if(isalpha(option[last_index + 1]) == 0)
       return 0;

   return 1;
}

int _libarg_count_parameters(int offset, int argc, char *argv[],
                                            struct LibargCallbacks callbacks) {
    int index = 0;
    int counted = 0;
    int expected = callbacks.parameters(argv[offset]);

    /* Requires at least one parameter */
    if(expected == LIBARG_VARIABLE_ONE) {
        expected = 1;
    }

    for(index = offset + 1; index < argc && counted != expected; index++) {
        char *argument = argv[index];

        /* Stop at the next option */
        if(callbacks.is_option(argument) == 1)
            break;

        counted++;
    }

    return counted;
}

int _libarg_count_positionals(int argc, char *argv[],
                                            struct LibargCallbacks callbacks) {
    int index = 0;
    int counted = 0;

    for(index = 1; index < argc; index++) {
        char *argument = argv[index];

        if(callbacks.is_option(argument) == 1) {
            index += _libarg_count_parameters(index, argc, argv, callbacks);

            continue;
        }

        counted++;
    } 

    return counted;
}

void _libarg_options_check(int argc, char *argv[],
                                            struct LibargCallbacks callbacks) {
    int index = 0;

    for(index = 1; index < argc; index++) {
        int parameters = 0;
        int got_parameters = 0;
        int is_option = callbacks.is_option(argv[index]);

        if(is_option == 0)
            continue;

        parameters = callbacks.parameters(argv[index]);

        /* Unknown option; must error */
        if(parameters == LIBARG_UNKNOWN_OPTION)
            callbacks.error(argv[index], LIBARG_ERR_UNKNOWN_OPTION, 0, 0);


        got_parameters = _libarg_count_parameters(index, argc, argv,
                                                  callbacks);

        /* Variable parameters means none are needed minimum */
        if(parameters == LIBARG_VARIABLE)
            continue;

        /* Variable parameters, but at least one */
        if(parameters == LIBARG_VARIABLE_ONE) {
            if(got_parameters >= 1)
                continue;

            callbacks.error(argv[index], LIBARG_ERR_PARAM_COUNT, 1,
                            got_parameters);

            return;
        }

        /* Correct number of parameters */
        if(parameters == got_parameters)
            continue;

        callbacks.error(argv[index], LIBARG_ERR_PARAM_COUNT, parameters,
                        got_parameters);

        return;
    }
}

int _libarg_find_option(const char *option, int argc, char *argv[],
                                            struct LibargCallbacks callbacks) {
    int index = 0;

    /* Must be an option */
    if(callbacks.is_option(option) == 0) {
        fprintf(stderr, "libarg_find_option: attempt to find non-option '%s'"
                        "\n", option);
        exit(EXIT_FAILURE);
    }

    /* Probe argv for the option */
    for(index = 0; index < argc; index++) {
        if(strcmp(option, argv[index]) != 0)
            continue;

        return index;
    }

    return LIBARG_UNKNOWN_OPTION;
}
