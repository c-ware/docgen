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
 * The backend for converting 'function' tokens into manual pages.
*/

#include "../functions.h"

void docgen_functions_manpage(struct DocgenArguments arguments, struct DocgenFunction function) {
    int index = 0;
    FILE *location = NULL;
    char file_path[LIBPATH_MAX_PATH + 1];

    /* Open the file */
    docgen_create_file_path(arguments, function.name, file_path, LIBPATH_MAX_PATH);
    libpath_join_path(file_path, LIBPATH_MAX_PATH, "./doc/", function.name,
                      ".", arguments.section, NULL);
    location = fopen(file_path, "w");

    /* Write the top header */
    fprintf(location, ".TH \"%s\" \"%s\" \"%s\" \"\" \"%s\"\n",
            function.name, arguments.section, arguments.date,
            arguments.title);

    /* 'Name' section */
    fprintf(location, "%s", ".SH NAME\n");
    fprintf(location, "%s - %s\n", function.name, function.brief);

    /* Synopsis */
    fprintf(location, "%s", ".SH SYNOPSIS\n");
    
    /* Inclusions */
    for(index = 0; index < carray_length(arguments.inclusions); index++) {
        struct Inclusion inclusion = arguments.inclusions->contents[index];

        if(inclusion.type == DOCGEN_INCLUSION_LOCAL) {
            fprintf(location, ".B \"#include \"\"%s\"\"\"\n.br\n", inclusion.path);
        } else if(inclusion.type == DOCGEN_INCLUSION_SYSTEM) {
            fprintf(location, ".B #include <%s>\n.br\n", inclusion.path);
        }
    }

    for(index = 0; index < carray_length(function.inclusions); index++) {
        struct Inclusion inclusion = function.inclusions->contents[index];

        if(inclusion.type == DOCGEN_INCLUSION_LOCAL) {
            fprintf(location, ".B \"#include \"\"%s\"\"\"\n.br\n", inclusion.path);
        } else if(inclusion.type == DOCGEN_INCLUSION_SYSTEM) {
            fprintf(location, ".B #include <%s>\n.br\n", inclusion.path);
        }
    }

    /* "void" return type */
    if(function.return_data.return_type[0] == '\0')
        strcat(function.return_data.return_type, "void");

    /* Function signature */
    fprintf(location, "\n\\fB%s %s(", function.return_data.return_type,
            function.name);

    /* Generate the arguments and parameters */
    for(index = 0; index < carray_length(function.parameters); index++) {
        int is_ptr = 0;
        struct DocgenFunctionParameter parameter;

        parameter = function.parameters->contents[index];

        if(strchr(parameter.type, '*') != NULL)
            is_ptr = 1;

        /* Decide whether or not to display the trailing comma */
        if(index == carray_length(function.parameters) - 1) {
            if(is_ptr == 0) {
                fprintf(location, "%s \\fI%s\\fB", parameter.type, parameter.name);
            } else {
                fprintf(location, "%s\\fI%s\\fB", parameter.type, parameter.name);
            }
        } else {
            if(is_ptr == 0) {
                fprintf(location, "%s \\fI%s\\fB, ", parameter.type,
                        parameter.name);
            } else {
                fprintf(location, "%s\\fI%s\\fB, ", parameter.type,
                        parameter.name);
            }
        }
    }

    /* If theres no parameters, simply do void */
    if(carray_length(function.parameters) == 0)
        fprintf(location, "%s", "void");
        
    fprintf(location, "%s", ");\n.br\n\n\\fR");

    /* Description */
    fprintf(location, "%s", "\n.SH DESCRIPTION\n");
    fprintf(location, "%s\n", function.description);

    /* Description error list */
    if(carray_length(function.errors) == 0)
        fprintf(location, "%s", "This function has no error conditions.\n.br\n\n");
    else {
        fprintf(location, "%s", "This function will display an error to the "
                "stderr, and abort the program if any of following conditions "
                "are met.\n");

        fprintf(location, "%s", ".RS\n");

        for(index = 0; index < carray_length(function.errors); index++) {
            fprintf(location, "o    %s\n", function.errors->contents[index].description);
            fprintf(location, "%s", ".br\n");
        }

        fprintf(location, "%s", ".RE\n\n");
    }

    /* Description parameter list */
    if(carray_length(function.parameters) == 0)
        fprintf(location, "%s", "This function has no parameters.\n");
    else {
        for(index = 0; index < carray_length(function.parameters); index++) {
            struct DocgenFunctionParameter parameter;

            parameter = function.parameters->contents[index];

            fprintf(location, "\\fI%s\\fR will be %s\n", parameter.name,
                    parameter.description);

            fprintf(location, "%s", ".br\n");
        }
    }

    /* Return value */
    fprintf(location, "%s", ".SH RETURN VALUE\n");
    
    if(function.return_data.return_value[0] == '\0')
        fprintf(location, "%s", "This function has no return value.\n");
    else {
        fprintf(location, "This function will return %s\n",
                function.return_data.return_value);
    }

    fprintf(location, "%s", ".SH NOTES\n");

    /* Notes (if any exist) */
    if(function.notes[0] != '\0')
        fprintf(location, "%s\n", function.notes);
    else
        fprintf(location, "%s", "This function has no notes.\n");

    /* Examples */
    fprintf(location, "%s", ".SH EXAMPLES\n");
    
    if(function.example[0] == '\0')
        fprintf(location, "%s", "This function has no examples.\n");
    else {
        /* Add breaks between each line */
        for(index = 0; function.example[index] != '\0'; index++) {
            fprintf(location, "%c", function.example[index]);

            if(function.example[index] == '\n') {
                fprintf(location, "%s", ".br\n");
            }
        }
    }

    /* See also */
    fprintf(location, "%s", ".SH SEE ALSO\n");

    if(carray_length(function.references) == 0)
        fprintf(location, "%s", "This function has no references.\n");
    else {
        struct Reference reference;

        index = 0;

        while(index < carray_length(function.references) - 1) {
            reference = function.references->contents[index];

            fprintf(location, "\\fB%s\\fR(%s), ", reference.manual,
                    reference.section);
            index++;
        }

        reference = function.references->contents[index];
        
        /* Print the last one */
        fprintf(location, "\\fB%s\\fR(%s)\n", reference.manual,
                reference.section);
        fprintf(location, "%s", ".br\n");
    }

    fclose(location);
}
