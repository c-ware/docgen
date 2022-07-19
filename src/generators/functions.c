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
 * This file works on generating functions for postprocessors. This will
 * essentially fill up a data structure that is passed to the post processor.
*/

#include "../docgen.h"

#include "generators.h"

#include "../extractors/macros/macros.h"
#include "../extractors/functions/functions.h"
#include "../extractors/structures/structures.h"
#include "../extractors/macro_functions/macro_functions.h"

struct PostprocessorData docgen_generate_functions(struct DocgenFunction function,
                                                   struct GeneratorParams parameters) {
    int index = 0;
    struct CString buffer_string;
    struct PostprocessorData data;

    INIT_VARIABLE(data);
    INIT_VARIABLE(buffer_string);

    /* Metadata data-- the target_structure field will be filled
     * in by the caller. */
    data.target = DOCGEN_TARGET_FUNCTION;
    data.target_structure = NULL; 

    /* Synopsis setup-- Functions have no 'arguments' like a project
     * might (command line arguments). */
    data.arguments = data.arguments;
    data.embedded_macros = make_embedded_macros(function.function_briefs, *(parameters.macros), *(function.embeds));
    data.embedded_structures = make_embedded_structures(*(parameters.structures),*(function.embeds));
    data.embedded_macro_functions = make_embedded_macro_functions(function.macro_function_briefs,
                                                                  *(parameters.macro_functions),
                                                                  *(function.embeds));
    return data;
}
