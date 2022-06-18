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

#ifndef CWARE_LIBARG_H
#define CWARE_LIBARG_H

#define CWARE_LIBARG_VERSION 1.0.1

#ifndef _LIBARG_CALLBACK_STRUCT
#define _LIBARG_CALLBACK_STRUCT libarg_callbacks
#endif

#ifndef _LIBARG_ARGV
#define _LIBARG_ARGV argv
#endif

#ifndef _LIBARG_ARGC
#define _LIBARG_ARGC argc
#endif

/* Error types */
#define LIBARG_ERR_UNKNOWN_OPTION   1
#define LIBARG_ERR_ARG_COUNT        2
#define LIBARG_ERR_PARAM_COUNT      3
#define LIBARG_ERR_HELP             4

/* Option types */
#define LIBARG_VARIABLE         -1
#define LIBARG_VARIABLE_ONE     -2
#define LIBARG_UNKNOWN_OPTION   -3

/* Macro wrappers */
#define libarg_count_parameters(offset)                            \
    _libarg_count_parameters((offset), _LIBARG_ARGC, _LIBARG_ARGV, \
                             _LIBARG_CALLBACK_STRUCT)

#define libarg_count_positionals() \
    _libarg_count_positionals(_LIBARG_ARGC, _LIBARG_ARGV, \
                              _LIBARG_CALLBACK_STRUCT)

#define libarg_options_check() \
    _libarg_options_check(_LIBARG_ARGC, _LIBARG_ARGV, _LIBARG_CALLBACK_STRUCT)

#define libarg_find_option(option)                          \
    _libarg_find_option(option, _LIBARG_ARGC, _LIBARG_ARGV, \
                        _LIBARG_CALLBACK_STRUCT)

/*
 * The functions that the libarg functions will use.
*/
struct LibargCallbacks {
    int (*is_option)(const char *option);
    int (*parameters)(const char *option);
    void (*error)(const char *argument, int type, int expected, int got);
};

/*
 * Detects options using UNIX style options, which includes the
 * POSIX style (-e), and GNU style options (--example).
 *
 * @param option: the option to check
 * @return: 1 if its an option, 0 if its not an option
*/
int libarg_is_unix_option(const char *option);

/*
 * Counts the number of parameters that an option at a given index has.
 * This assumes the option exists and makes no attempt to do error
 * correction. Counts parameters for both options with a fixed number
 * of arguments, and variable options.
 *
 * @param offset: the index in the argv to look for
 * @param argc: the argument count
 * @param argv: the argument vector
 * @param callbacks: the functions that libarg uses internally
 * @return: the number of option parameters
*/
int _libarg_count_parameters(int offset, int argc, char *argv[],
                                            struct LibargCallbacks callbacks);

/*
 * Counts the number of positional arguments inside of the argv. This
 * function does not perform any error checking on its own. Rather, it
 * should be used by an error checking interface. Please see the manual
 * for more information.
 *
 * @param argc: the argument count
 * @param argv: the argument vector
 * @param callbacks: the functions that libarg uses internally
 * @return: the number of positional arguments
*/
int _libarg_count_positionals(int argc, char *argv[],
                                            struct LibargCallbacks callbacks);

/*
 * Performs error checking for options. Specifically, performs error
 * checks for each argument that the is_option function in the callbacks
 * structure returns 1 for. Please see the manual for more information.
 *
 * @param argc: the argument count
 * @param argv: the argument vector
 * @param callbacks: the functions that libarg uses internally
*/
void _libarg_options_check(int argc, char *argv[],
                                            struct LibargCallbacks callbacks);

/*
 * Performs a linear search on the argv to determine the location of
 * an option in the argv, returning the index of the option.
 *
 * @param option: the option to search for
 * @param argc: the argument count
 * @param argv: the argument vector
 * @param callbacks: the functions that libarg uses internally
 * @return: the index of the option, or LIBARG_UNKNOWN_OPTION
*/
int _libarg_find_option(const char *option, int argc, char *argv[],
                                            struct LibargCallbacks callbacks);


#endif
