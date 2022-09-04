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
 * Implementations of the function embedding logic.
*/

#include <stdio.h>

#include "../../../docgen.h"

#include "../main.h"
#include "../../../common/parsing/parsing.h"

void compile_function_embed(struct ProgramState *state, int docgen_start_index) {
    int line_index = docgen_start_index;

    VERIFY_PROGRAM_STATE(state);
    LIBERROR_IS_NEGATIVE(docgen_start_index);
    LIBERROR_OUT_OF_BOUNDS(line_index + 2, carray_length(state->input_lines));
    VERIFY_CSTRING(&(state->input_lines->contents[line_index + 2]));

    /* Since we always start at the start of the docgen body (at "@docgen_start", we
     * can get the name and description first from here. */
    cstring_concats(&state->temp_function.name, strchr(state->input_lines->contents[line_index + 2].contents, ' ') + 1);
    cstring_concats(&state->temp_function.description, strchr(state->input_lines->contents[line_index + 3].contents, ' ') + 1);

    for(line_index = docgen_start_index; line_index < carray_length(state->input_lines); line_index++) {
        struct CString line;
    
        LIBERROR_OUT_OF_BOUNDS(line_index, carray_length(state->input_lines));
        VERIFY_CSTRING(&(state->input_lines->contents[line_index]));
        LIBERROR_IS_NULL(strchr((state->input_lines->contents[line_index].contents), '@'));

        line = state->input_lines->contents[line_index];
        common_parse_read_tag(line, &(state->tag_name)); 

        VERIFY_CSTRING(&(state->tag_name));

        if(strcmp(state->tag_name.contents, DOCGEN_END) == 0)
            break;

        /* Get a parameter */
        if(strcmp(state->tag_name.contents, "@fparam") == 0) {
            struct FunctionParameter parameter;

            /* Parameter name */
            parameter.name = cstring_init("");
            cstring_concats(&parameter.name, strchr(line.contents, ' ') + 1);

            /* Type of the parameter */
            line = state->input_lines->contents[line_index + 1];
            parameter.type = cstring_init("");
            cstring_concats(&parameter.type, strchr(line.contents, ' ') + 1);

            /* Description of the parameter */
            line = state->input_lines->contents[line_index + 2];
            parameter.description = cstring_init("");
            cstring_concats(&parameter.description, strchr(line.contents, ' ') + 1);

            carray_append(state->temp_function.parameters, parameter, FUNCTION_PARAMETER);

            continue;
        }

        /* Get the return type */
        if(strcmp(state->tag_name.contents, "@return") == 0) {
            cstring_concats(&state->temp_function.return_type, strchr(line.contents, ' ') + 1);

            continue;
        }
    }
}

void compile_function_embeds(struct ProgramState *state) {
    int line_index = 0;    

    VERIFY_PROGRAM_STATE(state);

    for(line_index = 0; line_index < carray_length(state->input_lines); line_index++) {
        struct CString line;
        int parameter_index = 0;
    
        LIBERROR_OUT_OF_BOUNDS(line_index, carray_length(state->input_lines));
        VERIFY_CSTRING(&(state->input_lines->contents[line_index]));
        LIBERROR_IS_NULL(strchr((state->input_lines->contents[line_index].contents), '@'));

        line = state->input_lines->contents[line_index];
        common_parse_read_tag(line, &(state->tag_name)); 

        VERIFY_CSTRING(&(state->tag_name));

        /* We only want to start generating an embed when we find the
         * docgen start tag. */
        if(strcmp(state->tag_name.contents, DOCGEN_START) != 0)
            continue;

        line = state->input_lines->contents[line_index + 1];
        common_parse_read_tag(line, &(state->tag_name)); 

        /* The line after the "@docgen_start" must be labeled as a 'function' if
         * we are to continue. */
        if(strcmp(strchr(line.contents, ' ') + 1, "function") != 0)
            continue;

        line = state->input_lines->contents[line_index + 2];
        common_parse_read_tag(line, &(state->tag_name)); 

        fprintf(state->compilation_output, "%s", "START_EMBED ");
        fprintf(state->compilation_output, "%s", strchr(line.contents, ' ') + 1);
        fprintf(state->compilation_output, "%c", '\n');

        /* Reset the temporary function */
        cstring_reset(&(state->temp_function.name));
        cstring_reset(&(state->temp_function.description));
        cstring_reset(&(state->temp_function.return_type));
        cstring_reset(&(state->temp_function.return_description));

        /* This could be replaced by a "carray_reset" macro */
        for(parameter_index = 0; parameter_index < carray_length(state->temp_function.parameters); parameter_index++) {
            FUNCTION_PARAMETER_FREE(state->temp_function.parameters->contents[parameter_index]); 
        }
        
        state->temp_function.parameters->length = 0;

        /* Compile the function at this location, and finish. The function is
         * given the index of the "@docgen_start" tag, since remember, we only
         * increment the line variable rather than the counter. */
        compile_function_embed(state, line_index); 

        fprintf(state->compilation_output, "/* %s */\n", state->temp_function.description.contents);

        /* Display return type and function name, with a return type that
         * is either void, or a type. */
        if(state->temp_function.return_type.length == 0)
            fprintf(state->compilation_output, "void %s(", state->temp_function.name.contents);
        else
            if(strchr(state->temp_function.return_type.contents, '*') != NULL)
                fprintf(state->compilation_output, "%s%s(", state->temp_function.return_type.contents, state->temp_function.name.contents);
            else
                fprintf(state->compilation_output, "%s %s(", state->temp_function.return_type.contents, state->temp_function.name.contents);

        /* Display Function parameters */
        for(parameter_index = 0; parameter_index < carray_length(state->temp_function.parameters); parameter_index++) {
            struct FunctionParameter parameter = state->temp_function.parameters->contents[parameter_index];

            /* Is the return type a pointer? */
            if(strchr(parameter.type.contents, '*') != NULL)
                fprintf(state->compilation_output, "%s%s", parameter.type.contents, parameter.name.contents);
            else
                fprintf(state->compilation_output, "%s %s", parameter.type.contents, parameter.name.contents);

            /* Do not add a ',' for the last parameter */
            if(parameter_index == carray_length(state->temp_function.parameters) - 1)
                continue;

            fprintf(state->compilation_output, "%c ", ',');
        }

        fprintf(state->compilation_output, "%s", ");\n");
        fprintf(state->compilation_output, "%s", "END_EMBED\n");

        carray_free(state->temp_function.parameters, FUNCTION_PARAMETER);
        state->temp_function.parameters = carray_init(state->temp_function.parameters, FUNCTION_PARAMETER);
    }
}
