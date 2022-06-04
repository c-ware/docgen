#ifndef CWARE_DOCGEN_EXTRACTOR_MACRO_FUNCTIONS_H
#define CWARE_DOCGEN_EXTRACTOR_MACRO_FUNCTIONS_H

#include "../../docgen.h"

/* Limits */
#define DOCGEN_MACRO_FUNCTION_NAME_LENGTH             256
#define DOCGEN_MACRO_FUNCTION_BRIEF_LENGTH            128
#define DOCGEN_MACRO_FUNCTION_EXAMPLE_LENGTH          2048
#define DOCGEN_MACRO_FUNCTION_DESCRIPTION_LENGTH      2048
#define DOCGEN_MACRO_FUNCTION_NOTES_LENGTH            2048

#define DOCGEN_MACRO_FUNCTION_INCLUSION_LENGTH                 128
#define DOCGEN_MACRO_FUNCTION_PARAMETER_NAME_LENGTH            32
#define DOCGEN_MACRO_FUNCTION_PARAMETER_DESCRIPTION_LENGTH     128
#define DOCGEN_MACRO_FUNCTION_ERROR_DESCRIPTION_LENGTH         128

/* Data structure properties */
#define MACRO_FUNCTION_PARAMETER_TYPE          struct DocgenMacroFunctionParameter
#define MACRO_FUNCTION_PARAMETER_HEAP          1
#define MACRO_FUNCTION_PARAMETER_FREE(value)

#define MACRO_FUNCTION_ERROR_TYPE          struct DocgenMacroFunctionError
#define MACRO_FUNCTION_ERROR_HEAP          1
#define MACRO_FUNCTION_ERROR_FREE(value)

#define MACRO_FUNCTION_TYPE          struct DocgenMacroFunction
#define MACRO_FUNCTION_HEAP          1
#define MACRO_FUNCTION_FREE(value)
#define MACRO_FUNCTION_COMPARE(cmp_a, cmp_b) (strcmp(cmp_a.name, cmp_b) == 0)

/*
 * Represents a parameter that is passed to a macro function, and
 * a description of it.
*/
struct DocgenMacroFunctionParameter {
    char name[DOCGEN_MACRO_FUNCTION_PARAMETER_NAME_LENGTH + 1];
    char description[DOCGEN_MACRO_FUNCTION_PARAMETER_DESCRIPTION_LENGTH + 1];
};

/*
 * An array of macro function parameters.
*/
struct DocgenMacroFunctionParameters {
    int length;
    int capacity;
    struct DocgenMacroFunctionParameter *contents;
};

/*
 * Represents an error that a macro function can produce.
*/
struct DocgenMacroFunctionError {
    char description[DOCGEN_MACRO_FUNCTION_PARAMETER_DESCRIPTION_LENGTH + 1];
};

/*
 * An array of function errors.
*/
struct DocgenMacroFunctionErrors {
    int length;
    int capacity;
    struct DocgenMacroFunctionError *contents;
};

/*
 * An abstract representation of all the information that is
 * provided about a macro function.
*/
struct DocgenMacroFunction {
    char name[DOCGEN_MACRO_FUNCTION_NAME_LENGTH + 1];
    char brief[DOCGEN_MACRO_FUNCTION_BRIEF_LENGTH + 1];
    char description[DOCGEN_MACRO_FUNCTION_DESCRIPTION_LENGTH + 1];
    char example[DOCGEN_MACRO_FUNCTION_EXAMPLE_LENGTH + 1];
    char notes[DOCGEN_MACRO_FUNCTION_NOTES_LENGTH + 1];

    struct DocgenMacroFunctionErrors *errors;
    struct DocgenMacroFunctionParameters *parameters;
    struct References *references;
    struct Inclusions *inclusions;
};

/*
 * An array of macro functions.
*/
struct DocgenMacroFunctions {
    int length;
    int capacity;
    struct DocgenMacroFunction *contents;
};

void docgen_extract_macro_functions_free(struct DocgenMacroFunctions *macro_functions);
struct DocgenMacroFunctions *docgen_extract_macro_functions(struct LibmatchCursor *cursor,
                                                  const char *comment_start,
                                                  const char *comment_end);

#endif
