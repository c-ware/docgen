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

#ifndef CWARE_DOCGEN_FUNCTIONS_H
#define CWARE_DOCGEN_FUNCTIONS_H

#include "../../docgen.h"
#include "../../extractors/functions/functions.h"

/* Backend selectors */
void docgen_functions_format(struct DocgenArguments arguments, struct DocgenFunction function);

/* Backends */
void docgen_functions_manpage(struct DocgenArguments arguments, struct DocgenFunction function);

/* Documentation generation */
/*
 * @docgen: function
 * @name: docgen_generate_functions
 * @brief: generate function manual pages
 *
 * @description
 * @Lorem ipsum dolor sit amet, consectetur adipiscing elit. Praesent ultricies
 * @pharetra ornare. Proin tristique nunc a sapien rutrum tempor. Sed vitae
 * @efficitur quam. Quisque tincidunt, turpis eu aliquet lobortis, diam nibh
 * @pellentesque odio, sit amet auctor dui elit in mauris. Maecenas at lacinia
 * @nisi. Aenean ac congue neque. In dapibus ornare nulla. Quisque imperdiet
 * @
 * @lectus vitae nisl aliquam mollis. Donec varius leo vel mattis venenatis.
 * @Donec eget elit tortor. Aenean commodo dui a augue aliquet, in convallis
 * @purus facilisis. In hac habitasse platea dictumst. Phasellus dignissim velit
 * @dictum, fringilla erat quis, pulvinar ipsum. Donec ultrices tempus lorem eu
 * @molestie.
 * @description
 *
 * @example
 * @#include <stdio.h>
 * @#include <stdlib.h>
 * @
 * @int main(void) {
 * @    printf("%s", "Hello, world!\\n");
 * @
 * @    return EXIT_SUCCESS;
 * @}
 * @example
 *
 * @param argc: the argument counter
 * @type: int
 * @param argv: the argument list
 * @type: char **
 * @param file: the file to read
 * @type: FILE *
 * @return: some stuff
 * @type: int *
 * 
 * @error: example error
 * @error: example error 2
 * @error: another example error
 * @reference: cware(cware)
 * @reference: other-ware(cware)
 * @reference: other-other-ware(cware)
*/
void docgen_functions_generate(struct DocgenArguments arguments, FILE *file);

#endif
