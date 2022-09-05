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
 * Implementations of the structure embedding logic.
*/

#include <stdio.h>

#include "../../../docgen.h"

#include "../main.h"
#include "../../../common/parsing/parsing.h"


void compile_structure_embed(struct ProgramState *state, int docgen_start_index) {
    int depth = 0;
    int line_index = docgen_start_index;
    struct CStrings *names = carray_init(names, CSTRING);

    VERIFY_PROGRAM_STATE(state);
    LIBERROR_IS_NEGATIVE(docgen_start_index);
    LIBERROR_OUT_OF_BOUNDS(line_index + 2, carray_length(state->input_lines));
    VERIFY_CSTRING(&(state->input_lines->contents[line_index + 2]));

    /* Basically, we want to iterate through each line until the end of the embed.
     * But for each "@struct_start" tag, we want to increase the indentation, and
     * do basic formatting. for each "@field", we do similar stuff. Each time a
     * "@struct_end" tag is found, we want to also pop off a name of the structure */
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

        /* Push this structure name onto the stack so when its finished
         * we can display its name, as nested structures are defined like:
         *
         * struct Foo {
         *      struct {
         *          ...
         *      } NestedStructure;
         * };
         *
         * Rather than
         *
         * struct Foo {
         *      struct NestedStructure {
         *          ...
         *      };
         * };
        */
        if(strcmp(state->tag_name.contents, "@struct_start") == 0) {
            int depth_index = 0;
            struct CString next_name = cstring_init("");

            depth++;

            /* Write the required number of spaces for the brief */
            for(depth_index = 0; depth_index < (depth * SPACING_PER_TAB); depth_index++) {
                fprintf(state->compilation_output, "%c", ' '); 
            }

            /* Write the brief */
            line = state->input_lines->contents[line_index + 2];
            common_parse_read_tag(line, &(state->tag_name)); 

            VERIFY_CSTRING(&(state->tag_name));

            fprintf(state->compilation_output, "/* %s */\n", strchr(line.contents, ' ') + 1);

            /* Write the required number of spaces for the name */
            for(depth_index = 0; depth_index < (depth * SPACING_PER_TAB); depth_index++) {
                fprintf(state->compilation_output, "%c", ' '); 
            }

            line = state->input_lines->contents[line_index + 1];
            common_parse_read_tag(line, &(state->tag_name)); 

            VERIFY_CSTRING(&(state->tag_name));

            /* Write the struct opening, and save the name of this structure. */
            cstring_concats(&next_name, strchr(line.contents, ' ') + 1);
            carray_append(names, next_name, CSTRING);

            fprintf(state->compilation_output, "%s", "struct {\n");
        }

        /* Pop the name of the next structure off the stack and decrease the depth */
        if(strcmp(state->tag_name.contents, "@struct_end") == 0) {
            int depth_index = 0;
            struct CString structure_name = names->contents[names->length - 1];

            /* Write the required number of spaces */
            for(depth_index = 0; depth_index < (depth * SPACING_PER_TAB); depth_index++) {
                fprintf(state->compilation_output, "%c", ' '); 
            }

            /* Write the struct name and closing */
            fprintf(state->compilation_output, "} %s;\n", structure_name.contents); 
            cstring_free(structure_name);

            depth--; 
            names->length--;
        }

        /* Display a field */
        if(strcmp(state->tag_name.contents, "@field") == 0) {
            int depth_index = 0;

            /* Write the required number of spaces for the comment */
            for(depth_index = 0; depth_index < ((depth + 1) * SPACING_PER_TAB); depth_index++) {
                fprintf(state->compilation_output, "%c", ' '); 
            }

            /* Write the brief */
            line = state->input_lines->contents[line_index + 2];
            common_parse_read_tag(line, &(state->tag_name)); 

            VERIFY_CSTRING(&(state->tag_name));

            fprintf(state->compilation_output, "/* %s */\n", strchr(line.contents, ' ') + 1); 

            /* Write the required number of spaces for the field */
            for(depth_index = 0; depth_index < ((depth + 1) * SPACING_PER_TAB); depth_index++) {
                fprintf(state->compilation_output, "%c", ' '); 
            }

            /* Display the type of the field */
            line = state->input_lines->contents[line_index + 1];
            common_parse_read_tag(line, &(state->tag_name)); 

            VERIFY_CSTRING(&(state->tag_name));

            fprintf(state->compilation_output, "%s", strchr(line.contents, ' ') + 1); 

            /* There should be a space between the type and name if the type is not a literal
             * pointer, as docgen uses x *y as opposed to x* y style pointers. */
            if(strchr(line.contents, '*') == NULL)
                fprintf(state->compilation_output, "%c", ' ');

            /* Display the name of the field */
            line = state->input_lines->contents[line_index];
            common_parse_read_tag(line, &(state->tag_name)); 

            VERIFY_CSTRING(&(state->tag_name));

            fprintf(state->compilation_output, "%s;\n", strchr(line.contents, ' ') + 1); 
        }
    }

    carray_free(names, CSTRING);
}

void compile_structure_embeds(struct ProgramState *state) {
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
        if(strcmp(strchr(line.contents, ' ') + 1, "structure") != 0)
            continue;

        line = state->input_lines->contents[line_index + 2];
        common_parse_read_tag(line, &(state->tag_name)); 

        fprintf(state->compilation_output, "%s", "START_EMBED ");
        fprintf(state->compilation_output, "%s", strchr(line.contents, ' ') + 1);
        fprintf(state->compilation_output, "%s", "\n");
        fprintf(state->compilation_output, "%s", "3\n");

        /* Since we always start at the start of the docgen body (at "@docgen_start", we
         * can get the description and name first from here. */
        fprintf(state->compilation_output, "/* %s */\nstruct %s {\n", strchr(state->input_lines->contents[line_index + 3].contents, ' ') + 1,
                                                                      strchr(state->input_lines->contents[line_index + 2].contents, ' ') + 1);


        compile_structure_embed(state, line_index);

        fprintf(state->compilation_output, "%s", "}\n");
        fprintf(state->compilation_output, "%s", "END_EMBED\n");

    }
}
