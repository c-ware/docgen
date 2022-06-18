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
