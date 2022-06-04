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
