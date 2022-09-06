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
 * Central header for docgen and all of its programs. Contains all the dependencies
 * for each program.
*/

#ifndef CWARE_DOCGEN_H
#define CWARE_DOCGEN_H

#include "deps/cstack/cstack.h"
#include "deps/carray/carray.h"
#include "deps/cstring/cstring.h"
#include "deps/liberror/liberror.h"
#include "deps/argparse/argparse.h"

#define FIELD_VALUE(x) \
    (strchr((x).contents, ' ') + 1)

#define PTR_TO_NUM(x) \
    ((unsigned long) (x))

#define CHAR_OFFSET(str, chr) \
    ((int) ((PTR_TO_NUM(chr)) - (PTR_TO_NUM(str))))

#define INVERT_BOOLEAN(x) \
        ((x) = !(x))

/* Object verifies (basically a bunch of calls to
 * liberror to verify the structure of an object
 * at runtime is how it should be. */
#define VERIFY_CSTRING(string)                \
    LIBERROR_IS_NULL((string));               \
    LIBERROR_IS_NULL((string)->contents);     \
    LIBERROR_IS_NEGATIVE((string)->length);   \
    LIBERROR_IS_NEGATIVE((string)->capacity); \
    LIBERROR_IS_VALUE((string)->capacity, 0)

#define VERIFY_CARRAY(carray)                \
    LIBERROR_IS_NULL((carray));              \
    LIBERROR_IS_NULL((carray)->contents);    \
    LIBERROR_IS_NEGATIVE((carray)->length);  \
    LIBERROR_IS_NEGATIVE((carray)->capacity)

#define VERIFY_COMMON_PARSE_LINE_STAT(line_stat)      \
    LIBERROR_IS_NULL((line_stat)->comment_start);     \
    LIBERROR_IS_NULL((line_stat)->comment_end);       \
    LIBERROR_IS_NULL((line_stat)->lastsingle_quote);  \
    LIBERROR_IS_NULL((line_stat)->lastdouble_quote)

/* Useful character classes */
#define CLASS_LOWER          "abcdefghijklmnopqrstuvwxyz"
#define CLASS_UPPER          "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
#define CLASS_NUMERIC        "0123456789"
#define CLASS_ALPHA          CLASS_LOWER CLASS_UPPER
#define CLASS_ALPHANUM       CLASS_LOWER CLASS_ALPHA CLASS_NUMERIC
#define CLASS_WHITESPACE     " \t\v\n\r"
#define CLASS_UNPRINTABLE    "\x0\x1\x2\x3\x4\x5\x6\x7\x8\x9\xA\xB\xC\xD\xE\xF\x10\x11\x12\x13\x14" \
                             "\x15\x16\x17\x18\x19\x1A\x1B\x1C\x1D\x1E\x1F\x20"
#define CLASS_PRINTABLE      "QWERTYUIOPASDFGHJKLZXCVBNMqwertyuiopasdfghjklzxcvbnm[];',./{}:\"<>?1234567890!@#$%^&*()-=_+`~\\|"
#define CLASS_NON_ALPHA      "[];',./{}:\"<>?!@#$%^&*()-=_+`~\\|" CLASS_NUMERIC CLASS_WHITESPACE CLASS_UNPRINTABLE
    

#endif
