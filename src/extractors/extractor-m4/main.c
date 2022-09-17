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
 * This file is the entry point of the program that filters all non-docgen code
 * out of the stdin, and will dump it back out. This does pretty much zero error
 * checking and leaves that up to the compiler, since its error checking is much
 * more approachable.
 *
 * It does this by scanning each line. If the line starts
 * with a single or double quote before the at-sign, regardless of whether or not the
 * string is closed, the line will be discarded.
 *
 * Assuming an at-sign appears before either (if either exist at all in the line), then
 * the line will be printed to the stdout after (and including) the at-sign in this format:
 * LINE_NUMBER:LINE
 *
 * This does, of course imply that a tag like this
 * "@type: foo"
 * will be ignored, but given how complex parsing becommes when we do not apply this small
 * limitation is, it is a sacrifice I think is fair to make. As an example of the complexity,
 * consider this circular rule.
 *  - We need to keep track of whether or not we are in a comment,
 *  - The opening delimiter can be inside of a string
 *  - We need to keep track of whether or not we are in a comment
 *
 * While this is technically solvable, even in a line-oriented parser, I am making the decision
 * to value the simplicity of this solution over the 100% perfectness of allowing it. It's
 * a 99.99% perfect solution. Who the hell would put quotes around them, anyway? Seriously if
 * you do that, go fix your damn code.
*/

#include <time.h>
#include <string.h>
#include <stdlib.h>

#include "../../docgen.h"
#include "../../common/errors/errors.h"
#include "../../common/parsing/parsing.h"

#include "main.h"

/*
 * This function will run through each line, and display any docgen tags
 * on that line if there are any.
*/
void display_docgen_tags(struct CStrings lines) {
    int line_index = 0;

    VERIFY_CSTRING(&lines);

    for(line_index = 0; line_index < carray_length(&lines); line_index++) {
        int tag_index = 0;
        struct CString line;

        LIBERROR_INIT(line);
        LIBERROR_OUT_OF_BOUNDS(line_index, lines.length);  
        LIBERROR_IS_NULL(lines.contents[line_index].contents);
        VERIFY_CSTRING(&lines.contents[line_index]);

        line = lines.contents[line_index];

        /* Ignore this line. Not a tag. */
        if(common_parse_line_has_tag(line) == 0)
            continue;

        tag_index = common_parse_get_tag_index(line);
        LIBERROR_IS_NEGATIVE(tag_index);
        LIBERROR_OUT_OF_BOUNDS(tag_index, line.length);

        printf("%i:%s\n", line_index + 1, line.contents + tag_index);
    }
}

int main(void) {
    struct CStrings *stdin_lines = carray_init(stdin_lines, CSTRING);

    common_parse_readlines(stdin_lines, stdin);

    display_docgen_tags(*stdin_lines);
    
    carray_free(stdin_lines, CSTRING);

    return EXIT_SUCCESS;
}
