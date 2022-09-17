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

#ifndef CWARE_DOCGEN_COMPILER_C_H
#define CWARE_DOCGEN_COMPILER_C_H

/* Configuration */
#define SPACING_PER_TAB 4

/* Tags */
#define DOCGEN_START    "@docgen_start"
#define DOCGEN_END      "@docgen_end"

/* Exit codes */
#define EXIT_UNCLOSED_DOCGEN            2
#define EXIT_INCOMPLETE_LINE_NUMBER     3
#define EXIT_EXPECTED_COLON             4
#define EXIT_EXPECTED_AT_SIGN           5
#define EXIT_EXPECTED_LINE_NUMBER       6
#define EXIT_EXPECTED_TEXT              7
#define EXIT_EMPTY_LINE                 8
#define EXIT_UNRECOGNIZED_MULTILINE     9
#define EXIT_UNCLOSED_TAG               10
#define EXIT_UNRECOGNIZED_TAG           11
#define EXIT_MISSING_LINES              12
#define EXIT_UNEXPECTED_TAG             13
#define EXIT_EXPECTED_TAG               14
#define EXIT_TAG_OUTSIDE_OF_GROUP       15
#define EXIT_EXPECTED_SPACE             16

/* Misc. information */
#define PROGRAM_NAME    "docgen-compiler-c"

#define VERIFY_PROGRAM_STATE(state) \
    LIBERROR_IS_NULL((state)->input_lines);        \
    LIBERROR_IS_NULL((state)->compilation_output); \
    VERIFY_CSTRING(&((state)->tag_name));          \
    VERIFY_CARRAY((state)->input_lines)

#define FUNCTION_PARAMETER_TYPE struct FunctionParameter
#define FUNCTION_PARAMETER_HEAP 1
#define FUNCTION_PARAMETER_FREE(value) \
    cstring_free(value.name);          \
    cstring_free(value.type);          \
    cstring_free(value.description)

#define MACRO_FUNCTION_PARAMETER_TYPE struct MacroFunctionParameter
#define MACRO_FUNCTION_PARAMETER_HEAP 1
#define MACRO_FUNCTION_PARAMETER_FREE(value) \
    cstring_free(value.name);          \
    cstring_free(value.description)

/* Represents a function parameter */
struct FunctionParameter {
    struct CString name;
    struct CString type;
    struct CString description;
};

/* Represents an array of function parameters */
struct FunctionParameters {
    int length;
    int capacity;
    struct FunctionParameter *contents;
};

/* Represents basic information about a function embed. */
struct Function {
    struct CString name;
    struct CString description;
    struct CString return_type;
    struct FunctionParameters *parameters;

    /* Unused. */
    struct CString return_description;
};

/* Represents a macro function parameter */
struct MacroFunctionParameter {
    struct CString name;
    struct CString description;
};

/* Represents an array of macro function parameters */
struct MacroFunctionParameters {
    int length;
    int capacity;
    struct MacroFunctionParameter *contents;
};

/* Represents basic information about a macro function embed. */
struct MacroFunction {
    struct CString name;
    struct CString description;
    struct MacroFunctionParameters *parameters;

    /* Unused. */
    struct CString return_description;
};

/* Container of state for the program. Contains common
 * data for memory reusage. */
struct ProgramState {
    struct Function temp_function;
    struct MacroFunction temp_macro_function;
    struct CString tag_name;
    struct CStrings *input_lines;
    FILE *compilation_output;
};

#endif
