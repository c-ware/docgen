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

#ifndef CWARE_LIBSTR_H
#define CWARE_LIBSTR_H

/*
 * Reverses a string in place.
 *
 * @param string: the string to reverse
 * @return: the reversed string
*/
char *strrev(char *string);

/*
 * Removes characters from a string in place.
 *
 * @param string: the string to remove from
 * @param characters: the characters to strip
 * @return: the number of characters stripped
*/
int strstrip(char *string, const char *characters);

/*
 * Counts the number of times a sub string appears in a
 * string.
 *
 * @param string: the string to count in
 * @param count: the sub string to count
 * @return: the number of occurrences
*/
int strcount(const char *string, const char *count);

/*
 * Replaces a string in a string with another
 * string in place.
 *
 * @param string: the string to replace
 * @param length: the maximum length of the buffer
 * @param find: the string to replace
 * @param replace: the string to replace with
 * @return: number of replacements
*/
int strrepl(char *string, const char *find, const char *replace, size_t size);

/*
 * Checks if a string starts with a sub string.
 *
 * @param string: the string to check
 * @param start: the sub string to compare
 * @return: 1 if string starts with the sub string, 0 if it does not
*/
int strstarts(const char *string, const char *start);

/*
 * C89 compliant semi-implementation of the snprintf function from C99.
 *
 * @param buffer: the buffer to write to
 * @param length: the maximum length of the buffer
 * @param format: the format string
*/
size_t ls_snprintf(char *buffer, size_t length, const char *format, ...);

#endif
