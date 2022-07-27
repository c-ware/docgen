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
 * @macros;generates manuals for each documented macro function
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
 * @The token type can be:
 * @    - structure
 * @    - project
 * @    - macro_function
 * @    - constant
 * @    - function
 * @    - category
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
 * @notes
 * @This is the notes section.
 * @notes
 *
 * @example
 * @This is the examples section.
 * @example
 *
 * @reference: cware(cware)
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "docgen.h"

/* Extractors */
#include "extractors/macros/macros.h"
#include "extractors/projects/projects.h"
#include "extractors/functions/functions.h"
#include "extractors/structures/structures.h"
#include "extractors/macro_functions/macro_functions.h"

/* Pipeline logic */
#include "generators/generators.h"
#include "postprocessors/writer/writer.h"
#include "postprocessors/postprocessors.h"

/* Global program state of reusable memory buffers. */
struct ProgramState program_state;

/*
 * @docgen: function
 * @brief: select a postprocessor to run based off the format
 * @name: select_postprocessor
 *
 * @description
 * @This function will select and run a postprocessor based off of the format that is
 * @given to it. It will extract the output from the post processor and return it to
 * @the caller who will then invoke the writer to do marker expansion.
 * @description
 *
 * @error: format is NULL
 *
 * @param format: the format we are generating documentation in
 * @type: const char *
 *
 * @param data: the data to past to the postprocessor
 * @type: struct PostprocessorData
 *
 * @param params: the parameters to pass to the postprocessor
 * @type: struct PostprocessorParams
 *
 * @return: the output from the postprocessor
 * @type: struct CString
*/
static struct CString select_postprocessor(const char *format, struct PostprocessorData data,
                                           struct PostprocessorParams params) {

    struct CString postprocessor_output;

    liberror_is_null(select_postprocessor, format);
    INIT_VARIABLE(postprocessor_output);;

    if(strcmp(format, "manpage") == 0)
        return docgen_postprocess_manual(data, params);

    liberror_unhandled(select_postprocessor);

    return postprocessor_output;
}

/*
 * @docgen: function
 * @brief: invoke the function generation on each function
 * @name: generate_functions
 *
 * @description
 * @This function is the front end for generating documentation for each
 * @function in the selected source file.
 * @description
 *
 * @param arguments: the parsed command line arguments
 * @type: struct DocgenArguments
 *
 * @param parameters: the generator parameters
 * @type: struct GeneratorParams
*/
void generate_functions(struct GeneratorParams parameters, struct DocgenArguments arguments) {
    int func_index = 0;
    char output_file_path[LIBPATH_MAX_PATH + 1] = "";

    for(func_index = 0; func_index < carray_length(parameters.functions); func_index++) {
        struct CString output;
        struct DocgenFunction function;
        struct PostprocessorParams params;
        struct PostprocessorData representation;
        FILE *output_file = NULL;

        INIT_VARIABLE(output);
        INIT_VARIABLE(params);
        INIT_VARIABLE(function);
        INIT_VARIABLE(representation);

        params.arguments = arguments;
        params.target = DOCGEN_TARGET_FUNCTION;
        params.target_structure = (void *) (&function);

        function = parameters.functions->contents[func_index];
        representation = docgen_generate_functions(function, parameters);
        output = select_postprocessor(arguments.format, representation, params);

        /* Open the output file, and perform the last stage of preprocessing
         * into the file. */
        if(strcmp(arguments.format, "manpage") == 0)
            libpath_join_path(output_file_path, LIBPATH_MAX_PATH, "doc", "/", function.name, ".", arguments.section, NULL);
        else
            liberror_unhandled(generate_functions);

        dump_cstring(arguments.format, output, output_file_path);

        /* Resource cleanup. Yes, we cast away const for a reason. We all know that
         * free(void *) should have actually been free(const void *). */
        cstring_free(output);
        free((char *) representation.brief);
        free((char *) representation.name);
        free((char *) representation.arguments);
        free((char *) representation.description);
        free((char *) representation.example);
        free((char *) representation.return_value);
        free((char *) representation.notes);
        carray_free(representation.embedded_functions, CSTRING);
        carray_free(representation.embedded_macro_functions, CSTRING);
        carray_free(representation.embedded_structures, CSTRING);
        carray_free(representation.embedded_macros, CSTRING);
    }
}

/*
 * @docgen: function
 * @brief: invoke the macro function generation on each macro function
 * @name: generate_functions
 *
 * @description
 * @This function is the front end for generating documentation for each
 * @macro function in the selected source file.
 * @description
 *
 * @param arguments: the parsed command line arguments
 * @type: struct DocgenArguments
 *
 * @param parameters: the generator parameters
 * @type: struct GeneratorParams
*/
void generate_macro_functions(struct GeneratorParams parameters, struct DocgenArguments arguments) {
    int mfunc_index = 0;
    char output_file_path[LIBPATH_MAX_PATH + 1] = "";

    for(mfunc_index = 0; mfunc_index < carray_length(parameters.macro_functions); mfunc_index++) {
        struct CString output;
        struct DocgenMacroFunction macro_function;
        struct PostprocessorParams params;
        struct PostprocessorData representation;
        FILE *output_file = NULL;

        INIT_VARIABLE(output);
        INIT_VARIABLE(params);
        INIT_VARIABLE(macro_function);
        INIT_VARIABLE(representation);

        params.arguments = arguments;
        params.target = DOCGEN_TARGET_MACRO_FUNCTION;
        params.target_structure = (void *) (&macro_function);

        macro_function = parameters.macro_functions->contents[mfunc_index];
        representation = docgen_generate_macro_functions(macro_function, parameters);
        output = select_postprocessor(arguments.format, representation, params);

        /* Open the output file, and perform the last stage of preprocessing
         * into the file. */
        if(strcmp(arguments.format, "manpage") == 0)
            libpath_join_path(output_file_path, LIBPATH_MAX_PATH, "doc", "/", macro_function.name, ".", arguments.section, NULL);
        else
            liberror_unhandled(generate_macro_functions);

        dump_cstring(arguments.format, output, output_file_path);

        /* Resource cleanup. Yes, we cast away const for a reason. We all know that
         * free(void *) should have actually been free(const void *). */
        cstring_free(output);
        free((char *) representation.brief);
        free((char *) representation.name);
        free((char *) representation.arguments);
        free((char *) representation.description);
        free((char *) representation.example);
        free((char *) representation.notes);
        carray_free(representation.embedded_macro_functions, CSTRING);
        carray_free(representation.embedded_functions, CSTRING);
        carray_free(representation.embedded_structures, CSTRING);
        carray_free(representation.embedded_macros, CSTRING);
    }
}

/*
 * @docgen: function
 * @brief: invoke project generation on the file
 * @name: generate_functions
 *
 * @description
 * @This function is the front end for generating documentation for any project
 * @in the selected source file.
 * @description
 *
 * @param arguments: the parsed command line arguments
 * @type: struct DocgenArguments
 *
 * @param parameters: the generator parameters
 * @type: struct GeneratorParams
*/
void generate_project(struct GeneratorParams parameters, struct DocgenArguments arguments) {
    struct CString output;
    struct PostprocessorParams params;
    struct PostprocessorData representation;
    char output_file_path[LIBPATH_MAX_PATH + 1] = "";
    FILE *output_file = NULL;

    INIT_VARIABLE(output);
    INIT_VARIABLE(params);
    INIT_VARIABLE(representation);

    params.arguments = arguments;
    params.target = DOCGEN_TARGET_PROJECT;
    params.target_structure = (void *) parameters.project;

    representation = docgen_generate_project(*(parameters.project), parameters);
    output = select_postprocessor(arguments.format, representation, params);

    /* Open the output file, and perform the last stage of preprocessing
     * into the file. */
    if(strcmp(arguments.format, "manpage") == 0)
        libpath_join_path(output_file_path, LIBPATH_MAX_PATH, "doc", "/", parameters.project->name, ".", arguments.section, NULL);
    else
        liberror_unhandled(generate_functions);

    dump_cstring(arguments.format, output, output_file_path);

    /* Resource cleanup. Yes, we cast away const for a reason. We all know that
     * free(void *) should have actually been free(const void *). */
    free((char *) representation.brief);
    free((char *) representation.name);
    free((char *) representation.arguments);
    free((char *) representation.description);

    cstring_free(output);
    carray_free(representation.embedded_functions, CSTRING);
    carray_free(representation.embedded_macro_functions, CSTRING);
    carray_free(representation.embedded_structures, CSTRING);
    carray_free(representation.embedded_macros, CSTRING);
}

int main(int argc, char **argv) {
    FILE *source_file = stdin;
    struct LibmatchCursor cursor;
    struct DocgenProject project;
    struct GeneratorParams generator_parameters;
    struct DocgenArguments arguments = main_parse(argc, argv);

    INIT_VARIABLE(cursor);
    INIT_VARIABLE(generator_parameters);

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

    /* Extract all potentially necessary tokens from the source file.
     * Remember that extractor functions will actually make a copy of
     * the cursor we pass to it. */
    cursor = libmatch_cursor_from_stream(source_file);
    generator_parameters.macros = docgen_extract_macros(&cursor, "/*", "*/");
    generator_parameters.functions = docgen_extract_functions(&cursor, "/*", "*/");
    generator_parameters.structures = docgen_extract_structures(&cursor, "/*", "*/");
    generator_parameters.macro_functions = docgen_extract_macro_functions(&cursor, "/*", "*/");
    generator_parameters.inclusions = arguments.inclusions;

    /* Extract the project. Unfortunately this cannot be put above since the project
     * extractor returns a stack structure rather than a pointer to one, so it cannot
     * be an opaque structure. This works OK, though.*/
    project = docgen_extract_project(&cursor, "/*", "*/");
    generator_parameters.project = &project;

    /* Determine which thing to generate documentation for */
    if(strcmp(arguments.category, "functions") == 0) {
        generate_functions(generator_parameters, arguments);
    } else if(strcmp(arguments.category, "macro_functions") == 0) {
        generate_macro_functions(generator_parameters, arguments);
    } else if(strcmp(arguments.category, "project") == 0) {
        generate_project(generator_parameters, arguments);
    }

    /* Program resource cleanup */
    fclose(source_file);
    libmatch_cursor_free(&cursor);
    carray_free(arguments.inclusions, INCLUDE);
    docgen_extract_macros_free(generator_parameters.macros);
    docgen_extract_functions_free(generator_parameters.functions);
    docgen_extract_structures_free(generator_parameters.structures);
    docgen_extract_macro_functions_free(generator_parameters.macro_functions);
    docgen_extract_project_free(&project);

    return EXIT_SUCCESS;
}
