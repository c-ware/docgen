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

#ifndef CWARE_DOCGEN_BACKEND_MANPAGE
#define CWARE_DOCGEN_BACKEND_MANPAGE

#define OUTPUT_FILE_PATH_LENGTH    1024 + 1

#define SECTION_TYPE    struct Section
#define SECTION_HEAP    1
#define SECTION_COMPARE(a, b) \
    (strcmp((a).name.contents, (b).name.contents) == 0)

#define SECTION_FREE(section)     \
    cstring_free((section).name); \
    cstring_free((section).body)

#define EMBED_TYPE    struct Embed
#define EMBED_HEAP    1
#define EMBED_COMPARE(a, b) \
    (strcmp((a).name.contents, (b).name.contents) == 0)

#define EMBED_FREE(embed)       \
    cstring_free((embed).name); \
    cstring_free((embed).body)

#define EMBED_REQUEST_TYPE  struct EmbedRequest
#define EMBED_REQUEST_HEAP  1
#define EMBED_REQUEST_FREE(request) \
    cstring_free((request).name);
#define EMBED_REQUEST_COMPARE(a, b) \
    (strcmp((a).name.contents, (b)) == 0)

#define REFERENCE_TYPE  struct Reference
#define REFERENCE_HEAP  1
#define REFERENCE_FREE(reference)      \
    cstring_free((reference).name);    \
    cstring_free((reference).category)

/* A section name and body pair */
struct Section {
    struct CString name;
    struct CString body;
};

struct Sections {
    int length;
    int capacity;
    struct Section *contents;
};

/* A parsed embed */
struct Embed {
    int type;
    struct CString name;
    struct CString body;

    /* This field is only used when embeds are being filtered, so
     * we do not need to make a new structure to hold embeds, and
     * whether or not they have a comment, as we merge non-commented
     * embeds of the same type and commented ones of the same type
     * separately before merging them both. */
    int has_comment;
};

struct Embeds {
    int length;
    int capacity;
    struct Embed *contents;
};

/* Embed requests */
struct EmbedRequest {
    int allow_comment;
    struct CString name;
};

struct EmbedRequests {
    int length;
    int capacity;
    struct EmbedRequest *contents;
};

/* References */
struct Reference {
    struct CString name;   
    struct CString category;
};

struct References {
    int length;
    int capacity;
    struct Reference *contents;
};


#endif
