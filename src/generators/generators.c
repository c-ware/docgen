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
 * This file works on generating data for postprocessors. This will essentially
 * fill up a data structure that is passed to the post processor.
*/

#include <assert.h>

#include "../docgen.h"

#include "generators.h"

#include "../extractors/macros/macros.h"
#include "../extractors/projects/projects.h"
#include "../extractors/functions/functions.h"
#include "../extractors/structures/structures.h"
#include "../extractors/macro_functions/macro_functions.h"

struct PostprocessorData docgen_generate_functions(struct DocgenFunction function,
                                                   struct GeneratorParams parameters) {
    int index = 0;
    struct Embed function_embed;
    struct CString buffer_string;
    struct PostprocessorData data;

    INIT_VARIABLE(data);
    INIT_VARIABLE(buffer_string);
    INIT_VARIABLE(function_embed);

    /* Metadata data-- the target_structure field will be filled
     * in by the caller. */
    data.brief = duplicate_string(function.brief);
    data.name = duplicate_string(function.name);

    /* Let's add the function we are documenting as an embed so it will follow the
     * same rules as the rest of the embeds! */
    function_embed.type = DOCGEN_EMBED_FUNCTION;
    strncat(function_embed.name, function.name, DOCGEN_EMBED_NAME_LENGTH);
    carray_append(function.embeds, function_embed, EMBED);

    /* Synopsis setup-- Functions have no 'arguments' like a project
     * might (command line arguments). */
    data.cli_inclusions = NULL;
    data.arguments = data.arguments;
    data.comment_inclusions = function.inclusions;
    data.embedded_macros = make_embedded_macros(function.macro_briefs, *parameters.macros, *function.embeds);
    data.embedded_structures = make_embedded_structures(function.structure_briefs, *parameters.structures, *function.embeds);
    data.embedded_macro_functions = make_embedded_macro_functions(function.macro_function_briefs, *parameters.macro_functions, *function.embeds);
    data.embedded_functions = make_embedded_functions(function.function_briefs, *parameters.functions, *function.embeds);

    /* Transfer sections (Functions have no arguments in their synopsis in the
     * same sense as a command line program project manual) */
    data.arguments = NULL;
    data.examples = duplicate_string(function.example);
    data.description = duplicate_string(function.description);
    data.return_value = duplicate_string(function.return_data.return_value);
    data.notes = duplicate_string(function.notes);
    data.see_also = function.references;

    return data;
}

struct PostprocessorData docgen_generate_project(struct DocgenProject project,
                                                 struct GeneratorParams parameters) {
    int index = 0;
    struct CString buffer_string;
    struct PostprocessorData data;

    INIT_VARIABLE(data);
    INIT_VARIABLE(buffer_string);

    /* Metadata data-- the target_structure field will be filled
     * in by the caller. */
    data.brief = duplicate_string(project.brief);
    data.name = duplicate_string(project.name);

    /* Synopsis setup-- projects have no in-comment inclusions like functions
     * and macro functions do. I really do not like this. Will have to make
     * them all be able to use both. */
    data.cli_inclusions = parameters.inclusions;
    data.comment_inclusions = NULL;
    data.embedded_macros = make_embedded_macros(project.macro_briefs, *parameters.macros, *project.embeds);
    data.embedded_structures = make_embedded_structures(project.structure_briefs, *parameters.structures, *project.embeds);
    data.embedded_macro_functions = make_embedded_macro_functions(project.macro_function_briefs, *parameters.macro_functions, *project.embeds);
    data.embedded_functions = make_embedded_functions(project.function_briefs, *parameters.functions, *project.embeds);

    /* Transfer sections. I really do not like how projects lack a lot of the same features
     * of the other targets. This is definitely something I will have to implement sooner
     * or later alongside in-comment inclusions. */
    data.arguments = duplicate_string(project.arguments);
    data.examples = "";
    data.description = duplicate_string(project.description);
    data.return_value = "";
    data.notes = "";
    data.see_also = project.references;

    return data;
}

struct PostprocessorData docgen_generate_macro_functions(struct DocgenMacroFunction macro_function,
                                                         struct GeneratorParams parameters) {
    int index = 0;
    struct Embed macro_function_embed;
    struct CString buffer_string;
    struct PostprocessorData data;

    INIT_VARIABLE(data);
    INIT_VARIABLE(buffer_string);
    INIT_VARIABLE(macro_function_embed);

    /* Metadata data-- the target_structure field will be filled
     * in by the caller. */
    data.brief = duplicate_string(macro_function.brief);
    data.name = duplicate_string(macro_function.name);

    /* Let's add the macro_function we are documenting as an embed so it will follow the
     * same rules as the rest of the embeds! */
    macro_function_embed.type = DOCGEN_EMBED_MACRO_FUNCTION;
    strncat(macro_function_embed.name, macro_function.name, DOCGEN_EMBED_NAME_LENGTH);
    carray_append(macro_function.embeds, macro_function_embed, EMBED);

    /* Synopsis setup-- Functions have no 'arguments' like a project
     * might (command line arguments). */
    data.cli_inclusions = NULL;
    data.arguments = data.arguments;
    data.comment_inclusions = macro_function.inclusions;
    data.embedded_macros = make_embedded_macros(macro_function.macro_briefs, *parameters.macros, *macro_function.embeds);
    data.embedded_structures = make_embedded_structures(macro_function.structure_briefs, *parameters.structures, *macro_function.embeds);
    data.embedded_macro_functions = make_embedded_macro_functions(macro_function.macro_function_briefs, *parameters.macro_functions, *macro_function.embeds);
    data.embedded_functions = make_embedded_functions(macro_function.function_briefs, *parameters.functions, *macro_function.embeds);


    /* Transfer sections (Functions have no arguments in their synopsis in the
     * same sense as a command line program project manual) */
    data.arguments = NULL;
    data.examples = duplicate_string(macro_function.example);
    data.description = duplicate_string(macro_function.description);
    data.return_value = "";
    data.notes = duplicate_string(macro_function.notes);
    data.see_also = macro_function.references;

    return data;
}
