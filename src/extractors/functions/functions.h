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

#define DOCGEN_TYPE_LENGTH                      64
#define DOCGEN_INCLUSION_LENGTH                 128
#define DOCGEN_PARAMETER_NAME_LENGTH            32
#define DOCGEN_PARAMETER_DESCRIPTION_LENGTH     128
#define DOCGEN_ERROR_DESCRIPTION_LENGTH         128

/* Data structure properties */
#define PARAMETER_TYPE          struct DocgenFunctionParameter
#define PARAMETER_HEAP          1
#define PARAMETER_FREE(value)

#define ERROR_TYPE          struct DocgenFunctionError
#define ERROR_HEAP          1
#define ERROR_FREE(value)

#define FUNCTION_TYPE          struct DocgenFunction
#define FUNCTION_HEAP          1
#define FUNCTION_FREE(value)
#define FUNCTION_COMPARE(cmp_a, cmp_b) (strcmp(cmp_a.name, cmp_b) == 0)

/*
 * Represents a parameter that is passed to a function, its type, and
 * a description of it.
*/
struct DocgenFunctionParameter {
    char name[DOCGEN_PARAMETER_NAME_LENGTH + 1];
    char type[DOCGEN_TYPE_LENGTH + 1];
    char description[DOCGEN_PARAMETER_DESCRIPTION_LENGTH + 1];
};

/*
 * An array of function parameters.
*/
struct DocgenFunctionParameters {
    int length;
    int capacity;
    struct DocgenFunctionParameter *contents;
};

/*
 * Represents an error that a function can produce.
*/
struct DocgenFunctionError {
    char description[DOCGEN_PARAMETER_DESCRIPTION_LENGTH + 1];
};

/*
 * An array of function errors.
*/
struct DocgenFunctionErrors {
    int length;
    int capacity;
    struct DocgenFunctionError *contents;
};

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

    struct DocgenFunctionErrors *errors;
    struct DocgenFunctionParameters *parameters;
    struct References *references;
    struct Inclusions *inclusions;
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
