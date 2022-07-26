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

#ifndef CWARE_DOCGEN_EXTRACTOR_FUNCTIONS_H
#define CWARE_DOCGEN_EXTRACTOR_FUNCTIONS_H

#include "../../docgen.h"

/* Limits */
#define DOCGEN_FUNCTION_NAME_LENGTH             256
#define DOCGEN_FUNCTION_BRIEF_LENGTH            128
#define DOCGEN_FUNCTION_RETURN_LENGTH           256
#define DOCGEN_FUNCTION_EXAMPLE_LENGTH          2048
#define DOCGEN_FUNCTION_DESCRIPTION_LENGTH      2048
#define DOCGEN_FUNCTION_NOTES_LENGTH            2048

#define DOCGEN_INCLUSION_LENGTH                 128
#define DOCGEN_PARAMETER_NAME_LENGTH            64
#define DOCGEN_PARAMETER_DESCRIPTION_LENGTH     128
#define DOCGEN_ERROR_DESCRIPTION_LENGTH         128

#define DOCGEN_FUNCTION_SETTING_LENGTH          32

/* Data structure properties */
#define FUNCTION_TYPE          struct DocgenFunction
#define FUNCTION_HEAP          1
#define FUNCTION_FREE(value)
#define FUNCTION_COMPARE(cmp_a, cmp_b) (strcmp(cmp_a.name, cmp_b) == 0)

/*
 * An abstract representation of all the information that is
 * provided about a function.
*/
struct DocgenFunction {
    char name[DOCGEN_FUNCTION_NAME_LENGTH + 1];
    char brief[DOCGEN_FUNCTION_BRIEF_LENGTH + 1];
    char description[DOCGEN_FUNCTION_DESCRIPTION_LENGTH + 1];
    char example[DOCGEN_FUNCTION_EXAMPLE_LENGTH + 1];
    char notes[DOCGEN_FUNCTION_NOTES_LENGTH + 1];

    struct {
        char return_value[DOCGEN_FUNCTION_RETURN_LENGTH + 1];
        char return_type[DOCGEN_TYPE_LENGTH + 1];
    } return_data;

    struct Errors *errors;
    struct Parameters *parameters;
    struct References *references;
    struct Inclusions *inclusions;
    struct Embeds *embeds;

    /* Function documentation settings */
    int function_briefs;
    int macro_function_briefs;
    int structure_briefs;
    int macro_briefs;
};

/*
 * An array of functions.
*/
struct DocgenFunctions {
    int length;
    int capacity;
    struct DocgenFunction *contents;
};

void docgen_extract_functions_free(struct DocgenFunctions *functions);
struct DocgenFunctions *docgen_extract_functions(struct LibmatchCursor *cursor,
                                                  const char *comment_start,
                                                  const char *comment_end);

#endif
