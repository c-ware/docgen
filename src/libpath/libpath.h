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

#ifndef CWARE_LIBCWPATH_H
#define CWARE_LIBCWPATH_H

/* Error checking macros */
#define __libpath_assert_nonnull(name, argument)                             \
do {                                                                         \
    if((argument) == NULL) {                                                 \
        fprintf(stderr, name ": argument '" #argument "' cannot be NULL\n"); \
        exit(EXIT_FAILURE);                                                  \
    }                                                                        \
} while(0)

#define __libpath_assert_nonnegative(name, argument)                 \
do {                                                                 \
    if((argument) < 0) {                                             \
        fprintf(stderr, name ": argument '" #argument "' cannot be " \
                        "negative\n");                               \
        exit(EXIT_FAILURE);                                          \
    }                                                                \
} while(0)

/* 
 * The maximum path lengths of various operating systems
 * These values will contain at *least* the correct number of bytes
 * to contain a path. They may be rounded up to a power of two. The
 * actual path length should be LIBPATH_MAX_PATH + 1, where the + 1
 * is for the NUL byte.
*/
#ifdef _MSDOS
    #define LIBPATH_MAX_PATH    65
#else
#ifdef _WIN32
    #define LIBPATH_MAX_PATH    260
#else
#ifdef __linux__
    #define LIBPATH_MAX_PATH    4096
#else
#ifdef __FreeBSD__
    #define LIBPATH_MAX_PATH    1024
#endif
#endif
#endif
#endif
#ifndef LIBPATH_MAX_PATH
#define LIBPATH_MAX_PATH 1024
#endif


/*
 * Joins a variable number of path segments into a single buffer,
 * while respecting the maximum length of the buffer. Length should
 * be the actual length of the buffer - 1, for the NUL byte. Last
 * argument should be NULL.
 *
 * @param buffer: the buffer to write to
 * @param length: the length of the buffer
 * @return: number of bytes written
*/
size_t libpath_join_path(char *buffer, int length, ...);

/*
 * Creates a new directory at a a certain path with a given
 * permissions set. Second parameter is ignored on MS-DOS.
 *
 * @param path: the path to make the directory at
 * @param mode: the mode to make the directory with
 * @return: 0 if it was successful, and -1 if it was unsuccessful
*/
int libpath_mkdir(const char *path, int mode);

/*
 * Determines whether or not the path provided exists or not.
 *
 * @param path: the path to check
 * @return: 1 if the path exists, 0 if it does not exist
*/
int libpath_exists(const char *path);











#endif
