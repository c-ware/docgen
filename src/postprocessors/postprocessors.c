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

#include <string.h>
#include <stdarg.h>

#include "../docgen.h"

#include "postprocessors.h"

#include "../extractors/macros/macros.h"
#include "../extractors/functions/functions.h"
#include "../extractors/structures/structures.h"
#include "../extractors/macro_functions/macro_functions.h"

/*
 * @docgen: function
 * @brief: add .br between each new line in a string and add it into a string
 * @name: add_breaks
 *
 * @description
 * @Troff requires a .br to signal a new line, so this function will add a  string
 * @into a cstring and translating each newline character (0x0A) into a newline
 * @and the '.br' string.
 * @description
 *
 * @error: string is NULL
 * @error: input is NULL
 *
 * @param string: the cstring to write the breaks to
 * @type: struct CString *
 *
 * @param input: the input string to translate
 * @type: const char *
*/
static void add_breaks(struct CString *string, const char *input) {
    int cindex = 0;

    liberror_is_null(add_breaks, string);
    liberror_is_null(add_breaks, input);

    /* For each newline character we find, output an extra string
     * '.br' to the cstring. */
    for(cindex = 0; cindex < string->length; cindex++) {
        char append_string[2] = "";

        append_string[0] = string->contents[cindex];
        append_string[1] = string->contents[cindex];

        if(string->contents[cindex] == '\n')
            cstring_concats(string, "\n.br");

        /* In the case of newlines, the fallthrough will cause
         * the final string append operation to have written
         * '\n.br\n' */
        cstring_concats(string, append_string);
    }
}

static void header(struct CString *string, struct PostprocessorData data,
                   struct PostprocessorParams params) {

    /* .TH "NAME" "SECTION" "DATE" "" "TITLE" */

    /* Make the title page */
    cstring_concats(string, ".TH \"");
    cstring_concats(string, data.name);
    cstring_concats(string, "\" \"");
    cstring_concats(string, params.arguments.section);
    cstring_concats(string, "\" \"");
    cstring_concats(string, params.arguments.date);
    cstring_concats(string, "\" \"");
    cstring_concats(string, params.arguments.title);
    cstring_concats(string, "\"\n");
}

static void name(struct CString *string, struct PostprocessorData data,
                 struct PostprocessorParams params) {

    /* NAME - BRIEF */
    cstring_concats(string, ".SH NAME\n");
    cstring_concats(string, data.name);
    cstring_concats(string, " - ");
    cstring_concats(string, data.brief);
    cstring_concats(string, "\n");
}

/*
 * Synopsis logic. Yes, this really does need its own section, as the synopsis
 * is probably the most mind-numbing part of this entire file.
 *
 * One of the main problem when it comes to generating the synopsis is
 * adding correct whitespace for embeds. In particular, adding correct new
 * lines (0xA). This is a problem because there is no rules on which embeds
 * must be specified. If a user does not embed a structure, they just do not.
 *
 * Being able to pick and choose which things to embed means that we cannot
 * just print all the embeds of a category and slap a new line at the end of it.
 * Consider adding embeds in this order:
 *  - Constants
 *  - Macro functions
 *  - Structures
 *  - Functions
 *
 * Let's say that the user ONLY embedded constants. If we simply displayed all
 * of the embeds for each type and put a new line, then we would have 3 extra
 * new lines after the constants, which looks.. weird.
 *
 * Now, let's say you thought about it for a bit, and decided "let's only add
 * a new line when there are embeds of a certain type." This is smarter,
 * though it still produces extra lines than necessary because there might
 * not be any other embed types after it. This means we need to take into
 * account the lengths of the other embeds, which for all intents and purposes
 * is completely out of the question. We can do better than this.
 *
 * But how? Well, it is quite simple. All we must do is iterate through an
 * array of each embed array, and then determine which embed array has data
 * in it. Finally, we put each embed with text in it into an array.
 *
 * We then iterate through this array, adding an extra empty line for each
 * group that is not the last one. Quite simple.
*/

static void display_embeds(struct CString *string, int length, ...) {
    int index = 0;
    int written = 0;
    int iter = 0;
    va_list embeds;
    struct CStrings *filtered_embeds[EMBED_TYPES];

    INIT_VARIABLE(embeds);
    INIT_VARIABLE(filtered_embeds);

    liberror_is_null(display_embeds, string);
    liberror_is_negative(display_embeds, length);
    liberror_is_number(display_embeds, length, "%d", 0);

    va_start(embeds, length);

    if(length > EMBED_TYPES)
        liberror_is_number(display_embeds, length, "%d", length);

    /* Determine which embeds have stuff-- we only want to display them */
    while(iter < length) {
        struct CStrings *array = va_arg(embeds, struct CStrings *);

        iter++;

        if(carray_length(array) == 0)
            continue;

        filtered_embeds[index] = array;

        index++;
        written++;
    }

    /* Iterate over each filtered embed group */
    for(index = 0; index < written; index++) {
        int embed_index = 0;
        struct CStrings *filtered_embed = filtered_embeds[index];

        /* Display the contents of each embed group */
        for(embed_index = 0; embed_index < carray_length(filtered_embed); embed_index++) {
            struct CString embed_string = filtered_embed->contents[embed_index];

            cstring_concats(string, embed_string.contents);

            /* Only add a new line if this is not the last embed */
            if(embed_index == (carray_length(filtered_embed) - 1))
                continue;

            /* However, if this is not the last embed, and there are
             * briefs, we should add another extra new line because having
             * comments directly before and after a function signature looks
             * weird. */
            if(strstr(embed_string.contents, "/*") == NULL)
                continue;

            cstring_concats(string, "\n");
        }

        /* Only the embed groups before the last should have a newline after */
        if(index == (written - 1))
            continue;

        cstring_concats(string, "\n");
    }

    va_end(embeds);

}

static void synopsis(struct CString *string, struct PostprocessorData data,
                     struct PostprocessorParams params) {
    liberror_is_null(synopsis, string);
    liberror_is_null(synopsis, params.target_structure);

    cstring_concats(string, ".SH SYNOPSIS\n");

    /* Projects can have an arguments string, while nothing else
     * can. */
    if(params.target == DOCGEN_TARGET_PROJECT) {
        cstring_concats(string, data.arguments);
        cstring_concats(string, "\n");
    }

    display_embeds(string, 4, data.embedded_macros, data.embedded_structures,
                   data.embedded_functions, data.embedded_macro_functions);
}

static void description(struct CString *string, struct PostprocessorData data,
                        struct PostprocessorParams params) {
    cstring_concats(string, ".SH DESCRIPTION\n");
    cstring_concats(string, data.description);
}

struct CString docgen_postprocess_manual(struct PostprocessorData data,
                                         struct PostprocessorParams params) {
    struct CString output = cstring_init("");

    header(&output, data, params);
    name(&output, data, params);
    synopsis(&output, data, params);
    description(&output, data, params);

    return output;
}










