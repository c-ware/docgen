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

#ifndef CWARE_DOCGEN_H
#define CWARE_DOCGEN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tags/tags.h"
#include "common/common.h"
#include "libstr/libstr.h"
#include "carray/carray.h"
#include "libarg/libarg.h"
#include "cstring/cstring.h"
#include "libpath/libpath.h"
#include "liberror/liberror.h"
#include "libmatch/libmatch.h"

#define CWARE_DOCGEN_VERSION    "1.0.2"

/* Limits */
#define DOCGEN_INCLUSION_TYPE_LENGTH    32 + 1
#define DOCGEN_EMBED_TYPE_LENGTH        32
#define DOCGEN_MANUAL_NAME_LENGTH       64 + 1
#define DOCGEN_MANUAL_SECTION_LENGTH    64 + 1
#define DOCGEN_INCLUSION_PATH_LENGTH    128 + 1
#define DOCGEN_EMBED_NAME_LENGTH        128
#define DOCGEN_SECTION_NAME_LENGTH      128
#define DOCGEN_TABLE_LENGTH             2048
#define DOCGEN_LINE_LENGTH              1024
#define DOCGEN_BLOCK_LINE_LENGTH        1024 + 1

/* The types of inclusions available */
#define DOCGEN_INCLUSION_LOCAL      0
#define DOCGEN_INCLUSION_SYSTEM     1

/* The types of data that can be embedded in an output file */
#define DOCGEN_EMBED_UNKNOWN        0
#define DOCGEN_EMBED_FUNCTION       1
#define DOCGEN_EMBED_STRUCTURE      2
#define DOCGEN_EMBED_CONSTANT       3
#define DOCGEN_EMBED_MACRO_FUNCTION 4

/* Types of target generators */
#define DOCGEN_TARGET_FUNCTION          0
#define DOCGEN_TARGET_MACRO_FUNCTION    1
#define DOCGEN_TARGET_PROJECT           2

/* Data structure properties */
#define INCLUDE_TYPE            struct Inclusion
#define INCLUDE_HEAP            1
#define INCLUDE_FREE(value)

#define REFERENCE_TYPE          struct Reference
#define REFERENCE_HEAP          1
#define REFERENCE_FREE(value)


#define EMBED_TYPE struct Embed
#define EMBED_HEAP 1
#define EMBED_FREE(value)

#define EMBED_COMPARE(cmp_a, cmp_b) \
    strcmp((cmp_a).name, (cmp_b).name) == 0

#define INIT_VARIABLE(v) \
    memset(&(v), 0, sizeof((v)))

/* Configuration */
#define DOCGEN_INDENTATION  4

/*
 * Represents an inclusion specified at the command line.
*/
struct Inclusion {
    int type;
    char path[DOCGEN_INCLUSION_PATH_LENGTH + 1];
};

/*
 * An array of inclusions.
*/
struct Inclusions {
    int length;
    int capacity;
    struct Inclusion *contents;
};

/*
 * Represents a 'reference' to another source of documentation. This is used
 * in the 'See Also' section.
*/
struct Reference {
    char manual[DOCGEN_MANUAL_NAME_LENGTH + 1];
    char section[DOCGEN_MANUAL_SECTION_LENGTH + 1];
};

/*
 * An array of references.
*/
struct References {
    int length;
    int capacity;
    struct Reference *contents;
};

/*
 * Represents a token to load into the manual page. This could
 * be a function, structure, constant, etc.
*/
struct Embed {
    char name[DOCGEN_EMBED_NAME_LENGTH + 1];
    int type;
};

/*
 * An array of embeds.
*/
struct Embeds {
    CARRAY_COUNTER_TYPE length;
    CARRAY_COUNTER_TYPE capacity;
    struct Embed *contents;
};

/*
 * Arguments for docgen.
*/
struct DocgenArguments {
    char *category;
    char *source;
    char *format;

    /* Extra information */
    char *section;
    char *title;
    char *date;
    const char *language;

    /* Inclusions to load into each manual */
    struct Inclusions *inclusions;
};

/*
 * A section in the documentation.
*/
struct Section {
    char name[DOCGEN_SECTION_NAME_LENGTH + 1];
    const char *body;
};

/*
 * An array of sections.
*/
struct Sections {
    int length;
    int capacity;
    struct Section *contents;
};

/*
 * Parameters for a post processor that has data that only the caller
 * should be able to access.
*/
struct PostprocessorParams {
    int target;
    void *target_structure;
    struct DocgenArguments arguments;
};

/*
 * Data to pass to a post processor to generate a CString
*/
struct PostprocessorData {
    /* Section metadata */
    const char *brief;
    const char *arguments;
    struct Inclusions *cli_inclusions;
    struct Inclusions *comment_inclusions;

    /* I really do not like this.
     * Function and macro errors could easily be unified under a
     * single banner (they are basically the exact samee structure),
     * and macro parameters are just function parameters without
     * a type. */
    struct DocgenFunctionErrors *func_errors;
    struct DocgenMacroFunctionErrors *mfunc_errors;
    struct DocgenFunctionParameters *func_parameters;
    struct DocgenMacroFunctionParameters *mfunc_parameters;

    struct CStrings *embedded_macros;
    struct CStrings *embedded_structures;
    struct CStrings *embedded_functions;
    struct CStrings *embedded_macro_functions;

    /* Every unique section in each type of manual (in no particular
     * order) */
    const char *name;
    const char *notes;
    const char *example;
    const char *description;
    const char *return_value;
    struct References *see_also;
};

/*
 * Parameters for a generator function.
*/
struct GeneratorParams {
    struct DocgenMacros *macros;
    struct DocgenProject *project;
    struct DocgenFunctions *functions;
    struct DocgenStructures *structures;
    struct DocgenMacroFunctions *macro_functions;

    struct Inclusions *inclusions;
};

/*
 *  Settings that the writer will determine internally to
 *  perform formatting.
*/
struct WriterParams {
    const char *bold_start;
    const char *bold_end;

    const char *italics_start;
    const char *italics_end;
};

/* Argument parsing and setup */
int main_parameters(const char *option);
void main_error(const char *option, int type, int expected, int got);
void main_extract_inclusions(int argc, char **argv, struct DocgenArguments *arguments);
struct DocgenArguments main_parse(int argc, char **argv);

#endif
