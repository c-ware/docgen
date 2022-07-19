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

#ifndef CWARE_DOCGEN_GENERATORS_H
#define CWARE_DOCGEN_GENERATORS_H

struct DocgenFunction;
struct GeneratorParams;
struct DocgenStructures;
struct PostprocessorData;

struct PostprocessorData docgen_generate_functions(struct DocgenFunction function, struct GeneratorParams parameters);

/*
 * common.c logic
*/

/*
 * @docgen: function
 * @brief: generate a string of each embedded macro request
 * @name: make_embedded_macros
 *
 * @include: generators.h
 *
 * @description
 * @This function will generate an array of CStrings that represent all the
 * @macros that can be embedded.
 * @description
 *
 * @param allow_briefs: whether or not briefs are allowed
 * @type: int
 *
 * @param macros: an array of macros in the source file
 * @type: struct DocgenMacros
 *
 * @param embeds: an array of embeds that something requests
 * @type: struct Embeds
 *
 * @return: an array of CStrings
 * @struct CStrings *
*/
struct CStrings *make_embedded_macros(int allow_briefs, struct DocgenMacros macros,
                                      struct Embeds embeds);

/*
 * @docgen: function
 * @brief: generate a string of each embedded structure request
 * @name: make_embedded_structures
 *
 * @include: generators.h
 *
 * @description
 * @This function will generate an array of CStrings that represent all the
 * @structures that were requested by the target.
 * @description
 *
 * @param allow_briefs: whether or not briefs should be displayed
 * @type: int
 *
 * @param structures: an array of structures in the source file
 * @type: struct DocgenStructures
 *
 * @param embeds: an array of embeds that something requests
 * @type: struct Embeds
 *
 * @return: an array of CStrings
 * @struct CStrings *
*/
struct CStrings *make_embedded_structures(int allow_briefs,
                                          struct DocgenStructures structures,
                                          struct Embeds embeds);
/*
 * @docgen: function
 * @brief: generate a string of each embedded macro function request
 * @name: make_embedded_macro_functions
 *
 * @include: generators.h
 *
 * @description
 * @This function will generate an array of CStrings that represent all the
 * @macro functions that were requested by the target.
 * @description
 *
 * @param allow_briefs: whether or not briefs should be allowed
 * @type: int
 *
 * @param macro_functions: an array of macro_functions in the source file
 * @type: struct DocgenMacroFunctions
 *
 * @param embeds: an array of embeds that something requests
 * @type: struct Embeds
 *
 * @return: an array of CStrings
 * @struct CStrings *
*/
struct CStrings *make_embedded_macro_functions(int allow_briefs,
                                               struct DocgenMacroFunctions macro_functions,
                                               struct Embeds embeds);

/*
 * @docgen: function
 * @brief: generate a string of each embedded function request
 * @name: make_embedded_functions
 *
 * @include: generators.h
 *
 * @description
 * @This function will generate an array of CStrings that represent all the
 * @functions that were requested by the target.
 * @description
 *
 * @param allow_briefs: whether or not briefs should be allowed
 * @type: int
 *
 * @param functions: an array of functions in the source file
 * @type: struct DocgenFunctions
 *
 * @param embeds: an array of embeds that something requests
 * @type: struct Embeds
 *
 * @return: an array of CStrings
 * @struct CStrings *
*/
struct CStrings *make_embedded_functions(int allow_briefs,
                                         struct DocgenFunctions functions,
                                         struct Embeds embeds);

#endif
