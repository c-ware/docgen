/*
 * @docgen: project
 * @name: docgen
 * @brief: generate documentation from c source code
 * 
 * @arguments
 * @docgen CATEGORY FILE [ --help | -h ] [ --format FORMAT | -f FORMAT ]
 * @                     [ --section SECTION | -s SECTION ]
 * @                     [ --title TITLE | -t TITLE ]
 * @                     [ --date DATE | -d DATE ]
 * @                     [ --include INCLUDE | -l INCLUDE ]
 * @                     [ --isystem INCLUDE | -i INCLUDE ]
 * @                     [ --language LANGUAGE | -x LANGUAGE ]
 * @arguments
 *
 * @embed macro_function: my_macro
 *
 * @description
 * @The 'docgen' project is a documentation generation system that uses
 * @a format-agnostic language to convert documentation into arbitrary
 * @formats that the user chooses, and the program has support for. This
 * @can make the creation of documentation easier, and less of a chore
 * @for the programmer because the documentation is stored in the source
 * @code of the program. This allows it to be more 'centralized,' and
 * @consistent across multiple formats.
 * @
 * @The program can generate different kinds of documentation. This can
 * @range anywhere from generating documentation for individual functions,
 * @and macro functions, to main project pages and categories to organize
 * @the project. What follows is a list of each type of token that documentation
 * @can be generated for, and a description of it.
 * @
 * @table
 * @sep: ;
 * @Token;Description
 * @functions;generates manuals for each documented function
 * @project;generates a manual for a 'file' which is usually used as its central manual
 * @category;alias for project, but is intended to be used for sub-manuals
 * @table
 * @
 * @Each type of token has different 'fields' in it that control how their
 * @manuals are generated. Some take an argument, some do not. The format
 * @that is shown in the examples should be followed strictly. This includes
 * @adhering to whitespace. Text in <>'s should be replaced with arguments.
 * @If a field name has a colon (:) at the end, that colon MUST be immediately
 * @followed by a space, then the argument.
 * @
 * @A token that should be extracted by docgen needs to be a multiline C comment
 * @where the second line has this format:
 * @
 * @@docgen: <TOKEN_TYPE>
 * @
 * @The token type can be a structure, project, constant, function, or category.
 * @
 * @A 'function' token will accept:
 * @table
 * @sep: ;
 * @Field;Description
 * @@name: <NAME>;the name of the function
 * @@brief: <DESCRIPTION>;a brief description of the function
 * @@param <NAME>: <DESCRIPTION>;the name of a parameter, and a brief description
 * @@type: <TYPE>;the type of a parameter, or return value
 * @@return: <DESCRIPTION>;a description of the return value
 * @@example;the start and end of the example block
 * @@description;the start and end of the description block
 * @@notes;the start and end of the notes block
 * @@error;an error that the function can raise
 * @@reference: <MANUAL>(<SECTION>);another piece of documentation to also see (see also section)
 * @table
 * @
 * @The tags 'return' and 'param' both require a type tag IMMEDIATELY after
 * @them. No empty lines or text in between them.
 * @
 * @A 'project' and 'category' token will accept:
 * @table
 * @sep: ;
 * @Field;Description
 * @@name: <NAME>;the name of the project
 * @@brief: <DESCRIPTION>;a brief description of the project
 * @@description;the start and end of the description block
 * @@include: <PATH>;local inclusion of a file
 * @@isystem: <PATH>;system inclusion of a file
 * @@table;the start and end of a table
 * @@sep: <SEPARATOR>;the separator for the table
 * @@embed <TYPE>: <NAME>;a token to embed into the synopsis
 * @@arguments;the arguments that the program can take (if this is a user application)
 * @@reference: <MANUAL>(<SECTION>);another piece of documentation to also see (see also section)
 * @@setting: func-briefs;show embedded function briefs
 * @table
 * @
 * @Tables must be embedded inside of descriptions. Because of this, the '@'
 * @symbol used for the @table tag will be the @ used to specify the lines of a
 * @block. Inside the body of a block, as described above, all lines of it must
 * have an '@' at the start.
 * @
 * @A 'structure' token will accept:
 * @table
 * @sep: ;
 * @Field;Description
 * @@name: <NAME>;the name of the structure or nested structure
 * @@brief: <DESCRIPTION>;a brief description of the structure or nested structure
 * @@field <NAME>: <DESCRIPTION>;the name and description of a field in the structure
 * @@type: <TYPE>;the type of the field
 * @@struct_start;start of a nested structure
 * @@struct_end;end of a nested structure
 * @table
 * @
 * @A 'constant' token will accept:
 * @table
 * @sep: ;
 * @Field;Description
 * @@name: <NAME>;the name of the constant
 * @@setting: ifndef;put an ifndef around the constant with the constant's name
 * @@brief: <DESCRIPTION>;a brief description of the constant
 * @@value: <VALUE>;the value of the constant
 * @table
 * @
 * @A 'macro_function' token will accept
 * @table
 * @sep: ;
 * @Field;Description
 * @@name: <NAME>;the name of the macro function
 * @@brief: <DESCRIPTION>;a brief description of the macro function
 * @@include: <PATH>;local inclusion of a file
 * @@isystem: <PATH>;system inclusion of a file
 * @@param <NAME>: <DESCRIPTION>;the name of a parameter, and a brief description
 * @@example;the start and end of the example block
 * @@description;the start and end of the description block
 * @@notes;the start and end of the notes block
 * @@error;an error that the macro function can raise
 * @table
 * @
 * @Do note that macro_function tokens do not accept a @type tag after the
 * @@param tag.
 * @
 * @The 'format' can be any of the following:
 * @table
 * @sep: ;
 * @Formatter;Description
 * @manpage;UNIX manual page for troff or nroff (default)
 * @dcfscript;DCF/SCRIPT or Waterloo SCRIPT for MVS or VM/CMS
 * @helpcms;VM/SP Release 5 HELPCMS file
 * @table
 * @
 * @Since you can embed function signatures, structures, constants, etc into the
 * @synopsis of the program, here is a list of each kind of token that can be
 * @embedded into the synopsis. Do note, however, that docgen needs to see the
 * @token's comment. If this is a problem, try using `cat file.c file.h | docgen`.
 * @As for tokens you can embed, here is an example of embedding each type of token:
 * @
 * @@embed structure: my_structure_name
 * @
 * @@embed constant: my_constant_name
 * @
 * @@embed function: my_function_name
 * @
 * @@embed macro_function: my_macro_function_name
 * @description
 *
 * @reference: cware(cware)
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "docgen.h"

#include "targets/projects/projects.h"
#include "targets/functions/functions.h"

/*
 * @docgen: macro_function
 * @brief: a macro that does something
 * @name: my_macro
 *
 * @description
 * @This is a macro function that does soemthing
 * @description
 *
 * @notes
 * @This macro is useless.
 * @notes
 *
 * @error: if you use it
 *
 * @param x: the x parameter
 * @param y: the y parameter
 * @param z: the z parameter
*/
#define my_macro(x, y, z) \
    ...


int main(int argc, char **argv) {
    int category = 0;
    FILE *source_file = stdin;
    struct DocgenArguments arguments = main_parse(argc, argv);

    category = main_enumerate(arguments.category);

    if(libpath_exists("doc") == 0) {
        fprintf(stderr, "%s", "docgen: could not find doc directory\n");
        exit(EXIT_FAILURE);
    }

    /* Use something other than stdin */
    if(strcmp(arguments.source, "-") != 0)
        source_file = fopen(arguments.source, "r");
 
    /* Could not find file */
    if(source_file == NULL) {
        fprintf(stderr, "docgen: could not open source file '%s' (%s)\n",
                arguments.source, strerror(errno));

        exit(EXIT_FAILURE);
    }


    /* Invoke the correct category generator */
    switch(category) {
        case DOCGEN_CATEGORY_FUNCTION:
            docgen_functions_generate(arguments, source_file);
            break;
        case DOCGEN_CATEGORY_PROJECT:
            docgen_project_generate(arguments, source_file);
            break;
        case DOCGEN_CATEGORY_CATEGORY:
            docgen_project_category_generate(arguments, source_file);
            break;
    }

    fclose(source_file);
    carray_free(arguments.inclusions, INCLUDE);

    return EXIT_SUCCESS;
}
