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
 * Implementations of the constant embedding logic.
*/

#include <stdio.h>

#include "../../../docgen.h"

#include "../main.h"
#include "../../../common/parsing/parsing.h"

void compile_constant_embed(struct ProgramState *state, int docgen_start_index) {
    int line_index = 0;    

    VERIFY_PROGRAM_STATE(state);

    /* Get the name and description */
    fprintf(state->compilation_output, "/* %s */\n", strchr(state->input_lines->contents[docgen_start_index + 3].contents, ' ') + 1);
    fprintf(state->compilation_output, "#define %s %s", strchr(state->input_lines->contents[docgen_start_index + 2].contents, ' ') + 1, "");

    for(line_index = docgen_start_index; line_index < carray_length(state->input_lines); line_index++) {
        struct CString line;
        int parameter_index = 0;
    
        LIBERROR_OUT_OF_BOUNDS(line_index, carray_length(state->input_lines));
        VERIFY_CSTRING(&(state->input_lines->contents[line_index]));
        LIBERROR_IS_NULL(strchr((state->input_lines->contents[line_index].contents), '@'));

        line = state->input_lines->contents[line_index];
        common_parse_read_tag(line, &(state->tag_name)); 

        VERIFY_CSTRING(&(state->tag_name));

        if(strcmp(state->tag_name.contents, DOCGEN_END) == 0)
            break;

        if(strcmp(state->tag_name.contents, "@value") == 0) {
            fprintf(state->compilation_output, "%s\n", strchr(line.contents, ' ') + 1);
        }
    }
}

void compile_constant_embeds(struct ProgramState *state) {
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

        /* The line after the "@docgen_start" must be labeled as a 'constant' if
         * we are to continue. */
        if(strcmp(strchr(line.contents, ' ') + 1, "constant") != 0)
            continue;

        line = state->input_lines->contents[line_index + 2];
        common_parse_read_tag(line, &(state->tag_name)); 

        /* Dump the name for the start embed, as well as the integer type */
        fprintf(state->compilation_output, "%s", "START_EMBED ");
        fprintf(state->compilation_output, "%s", strchr(line.contents, ' ') + 1);
        fprintf(state->compilation_output, "%c", '\n');
        fprintf(state->compilation_output, "%s", "0\n");

        compile_constant_embed(state, line_index);
        fprintf(state->compilation_output, "%s", "END_EMBED\n");

        carray_free(state->temp_macro_function.parameters, MACRO_FUNCTION_PARAMETER);
        state->temp_macro_function.parameters = carray_init(state->temp_macro_function.parameters, MACRO_FUNCTION_PARAMETER);
    }
}
