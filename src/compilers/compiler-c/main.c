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
 * This is the entry point of the docgen C compiler. It takes in tags that are
 * assumed to be in a C file, and translates them into a format readable by the
 * manual generator.
 *
 * A tag is just some sort of identifier that docgen can use. They start with an
 * '@' and then a name. Some tags can have data asssigned to them, and some
 * group data.
 *
 * The tags that this compiler uses fall into three main categories.
 *  - Fields
 *  - Multilines
 *  - Groups
 *
 * A field is a tag that conveys direct information about itself. You can think of
 * a field tag as a key/value pair, where the key is the tag name, and the value is
 * the information assigned to it. An example of one might be "@param". For example:
 * "@param: my_parameter" 
 *
 * A multiline tag is a tag that spans several lines. They are restrictive, having 
 * the text in between them being interpreted as raw text. Each line in between the
 * delimiters of a multiline must have their start signaled by an '@' at the start,
 * and there cannot be any lines in between without them. An example of one might
 * be "@description". Each multiline tag must have the name only, followed by a new
 * line. An example of "@description" would be:
 * 
 * "@description"
 * "@This is the description"
 * "@This is another line of the description"
 * "@description"
 *
 * A group tag is a tag similar to a multiline tag, but is much more freeform. They
 * are a way to group tags in a way that they are 'associated' with the group, and
 * together. They follow the same naming rules as multiline tags. As an example,
 * take the "@struct_start" and "@struct_end" tags.
 *
 * "@struct_start"
 * "@name: name of structure"
 * "@brief: brief description of structure"
 *
 * "@field: name of the field"
 * "@type: type of the field"
 * "@brief: brief description of field"
 * 
 * "@struct_end"
 *
 *
 *
 * Different tags are affected by different types of error checking. Error checking
 * is a fundamental part of the compiler, as we want to ensure that the program produces
 * helpful error messages for as many erroneous situations as possible.  That being said,
 * error checking can also often be verbose and long. We want to minimize the number of
 * functions we have to write, by making our code more generic. Even if it means that
 * we have to call them multiple times.
 *
 * What kind of error checking will this program need?
 *  - No empty lines
 *  - All lines have the prefix '\d+:@'
 *  - All tags are recognized
 *  - All tags, both multiline and group, are closed
 *  - Required tags follow the tags they are required by
 *  - Fields have text
 *
 * All of these errors must follow these rules, except for the lines denoted with [x],
 * where x is the rule number.
*/

#include <ctype.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>

#include "../../docgen.h"
#include "../../common/errors/errors.h"
#include "../../common/parsing/parsing.h"

#include "main.h"
#include "embeds/embeds.h"

/* 
 * =========================================
 *             Tag type checking
 * =========================================
*/
int is_multiline(struct CString tag) {
    VERIFY_CSTRING(&tag);

    if(strcmp(tag.contents, "@description") == 0)
        return 1;

    if(strcmp(tag.contents, "@synopsis") == 0)
        return 1;

    if(strcmp(tag.contents, "@notes") == 0)
        return 1;

    if(strcmp(tag.contents, "@examples") == 0)
        return 1;

    return 0;
}

int is_field(struct CString tag) {
    VERIFY_CSTRING(&tag);

    if(strcmp(tag.contents, "@value") == 0)
        return 1;

    if(strcmp(tag.contents, "@show_brief") == 0)
        return 1;

    if(strcmp(tag.contents, "@reference") == 0)
        return 1;

    if(strcmp(tag.contents, "@error") == 0)
        return 1;

    if(strcmp(tag.contents, "@mparam") == 0)
        return 1;

    if(strcmp(tag.contents, "@fparam") == 0)
        return 1;

    if(strcmp(tag.contents, "@mparam") == 0)
        return 1;

    if(strcmp(tag.contents, "@include") == 0)
        return 1;

    if(strcmp(tag.contents, "@field") == 0)
        return 1;

    if(strcmp(tag.contents, "@type") == 0)
        return 1;

    if(strcmp(tag.contents, "@name") == 0)
        return 1;

    if(strcmp(tag.contents, "@brief") == 0)
        return 1;

    if(strcmp(tag.contents, "@embed") == 0)
        return 1;

    if(strcmp(tag.contents, "@return") == 0)
        return 1;

    return 0;
}

int is_group(struct CString tag) {
    VERIFY_CSTRING(&tag);

    if(strcmp(tag.contents, "@docgen_start") == 0)
        return 1;

    if(strcmp(tag.contents, "@docgen_end") == 0)
        return 1;

    if(strcmp(tag.contents, "@struct_start") == 0)
        return 1;

    if(strcmp(tag.contents, "@struct_end") == 0)
        return 1;

    return 0;
}

/*
 * ========================
 * # Information retrival #
 * ========================
*/
int has_errors(struct ProgramState *state, int start_index) {
    int line_index = 0;

    VERIFY_PROGRAM_STATE(state);

    for(line_index = start_index; line_index < carray_length(state->input_lines); line_index++) {
        struct CString line;
 
        LIBERROR_OUT_OF_BOUNDS(line_index, carray_length(state->input_lines));
        VERIFY_CSTRING(&(state->input_lines->contents[line_index]));
        LIBERROR_IS_NULL(strchr((state->input_lines->contents[line_index].contents), '@'));

        line = state->input_lines->contents[line_index];
        common_parse_read_tag(line, &(state->tag_name)); 

        VERIFY_CSTRING(&(state->tag_name));

        /* Do not go past the end of the docgen block! */
        if(strcmp(state->tag_name.contents, DOCGEN_END) == 0)
            break;

        /* Regular error */
        if(strcmp(state->tag_name.contents, "@error") == 0) {
            return 1;
        }
    }

    return 0;
}


int has_parameters(struct ProgramState *state, int start_index) {
    int line_index = 0;

    VERIFY_PROGRAM_STATE(state);

    for(line_index = start_index; line_index < carray_length(state->input_lines); line_index++) {
        struct CString line;
    
        LIBERROR_OUT_OF_BOUNDS(line_index, carray_length(state->input_lines));
        VERIFY_CSTRING(&(state->input_lines->contents[line_index]));
        LIBERROR_IS_NULL(strchr((state->input_lines->contents[line_index].contents), '@'));

        line = state->input_lines->contents[line_index];
        common_parse_read_tag(line, &(state->tag_name)); 

        VERIFY_CSTRING(&(state->tag_name));

        /* Do not go past the end of the docgen block! */
        if(strcmp(state->tag_name.contents, DOCGEN_END) == 0)
            break;

        /* Function parameters */
        if(strcmp(state->tag_name.contents, "@fparam") == 0) {
            return 1;
        }

        /* Macro parameters */
        if(strcmp(state->tag_name.contents, "@mparam") == 0) {
            return 1;
        }
    }

    return 0;
}

int has_description(struct ProgramState *state, int start_index) {
    int line_index = 0;

    VERIFY_PROGRAM_STATE(state);

    for(line_index = start_index; line_index < carray_length(state->input_lines); line_index++) {
        struct CString line;
    
        LIBERROR_OUT_OF_BOUNDS(line_index, carray_length(state->input_lines));
        VERIFY_CSTRING(&(state->input_lines->contents[line_index]));
        LIBERROR_IS_NULL(strchr((state->input_lines->contents[line_index].contents), '@'));

        line = state->input_lines->contents[line_index];
        common_parse_read_tag(line, &(state->tag_name)); 

        VERIFY_CSTRING(&(state->tag_name));

        /* Do not go past the end of the docgen block! */
        if(strcmp(state->tag_name.contents, DOCGEN_END) == 0)
            break;

        /* Description tag */
        if(strcmp(state->tag_name.contents, "@description") == 0) {
            return 1;
        }
    }

    return 0;
}

/* 
 * =========================================
 *         Error checking components
 * =========================================
*/

/* Verify that each line has the prefix that matches the pattern '\d+:@' */
void error_lines_have_prefix(struct ProgramState *state) {
    int line_index = 0;

    VERIFY_PROGRAM_STATE(state);

    for(line_index = 0; line_index < state->input_lines->length; line_index++) {
        int char_index = 0;
        struct CString line;

        LIBERROR_OUT_OF_BOUNDS(line_index, state->input_lines->length);
        VERIFY_CSTRING(&(state->input_lines->contents[line_index]));
        LIBERROR_INIT(line);

        line = state->input_lines->contents[line_index];

        /* Must be at least one number. */
        for(char_index = 0; char_index < line.length; char_index++) {
            int character = -1;
            
            LIBERROR_OUT_OF_BOUNDS(char_index, line.length);
            character = line.contents[char_index];

            if(isdigit(character) != 0)
                continue;

            /* First character is not numeric, but that is OK if it is a colon.
             * If it is, stop the loop. */
            if(character == ':')
                break;

            /* Character is not numeric, and was not a colon */
            fprintf(LIBERROR_STREAM, PROGRAM_NAME ": first non-numeric character of line %i of input must be a colon (:), got '%c'\n", line_index + 1, character);
            exit(EXIT_INCOMPLETE_LINE_NUMBER);
        }

        /* If char_index is still 0, that means there was no number. */
        if(char_index == 0) {
            fprintf(LIBERROR_STREAM, PROGRAM_NAME ": line %i expected a line number\n", line_index + 1);
            exit(EXIT_EXPECTED_LINE_NUMBER);
        }

        /* Line is not missing a ':', it does not even have anything past this point */
        if((char_index + 1) > line.length) {
            fprintf(LIBERROR_STREAM, PROGRAM_NAME ": line %i expected ':' after line number, got the end of the line\n", line_index + 1);
            exit(EXIT_EXPECTED_COLON);
        }

        /* Next character must be a ':' */
        if(line.contents[char_index] != ':') {
            fprintf(LIBERROR_STREAM, PROGRAM_NAME ": line %i expected ':' after line number, got '%c'\n", line_index + 1, line.contents[char_index]);
            exit(EXIT_EXPECTED_COLON);
        }

        char_index++;

        /* Line is not missing a '@', it does not even have anything past this point */
        if((char_index + 1) > line.length) {
            fprintf(LIBERROR_STREAM, PROGRAM_NAME ": line %i expected '@' after colon, got the end of the line\n", line_index + 1);
            exit(EXIT_EXPECTED_AT_SIGN);
        }

        /* Next character must be a '@' */
        if(line.contents[char_index] != '@') {
            fprintf(LIBERROR_STREAM, PROGRAM_NAME ": line %i expected '@' after line number, got '%c'\n", line_index + 1, line.contents[char_index]);
            exit(EXIT_EXPECTED_COLON);
        }
    }
}

void error_all_tags_recognized(struct ProgramState *state) {
    int line_index = 0;
    int in_multiline = 0;

    VERIFY_PROGRAM_STATE(state);

    for(line_index = 0; line_index < carray_length(state->input_lines); line_index++) {
        struct CString line;

        LIBERROR_OUT_OF_BOUNDS(line_index, carray_length(state->input_lines));
        VERIFY_CSTRING(&(state->input_lines->contents[line_index]));

        line = state->input_lines->contents[line_index];
        common_parse_read_tag(line, &(state->tag_name)); 

        VERIFY_CSTRING(&(state->tag_name));

        /* Skip multilines so we do not attempt to read the line of a
         * multiline body, and the multiline tags themselves. */
        if(is_multiline(state->tag_name) == 1) {
            INVERT_BOOLEAN(in_multiline); 

            continue;
        }

        if(in_multiline == 1)
            continue;

        /* If we get here we know we are not in a multiline, so skip any
         * field tags, or group tags. Anything left over is not a tag we
         * recognize. */ 
        if(is_group(state->tag_name) == 1)
            continue;

        if(is_field(state->tag_name) == 1)
            continue;

        /* This is not a tag we recognize. */
        fprintf(LIBERROR_STREAM, PROGRAM_NAME ": unrecognized tag '%s' on line %i\n", state->tag_name.contents, line_index + 1);
        exit(EXIT_UNRECOGNIZED_TAG);
    }
}

void error_tag_is_closed(struct ProgramState *state, const char *start_tag, const char *end_tag) {
    int line_index = 0;
    int in_multiline = 0;
    int multiline_tag_line = 0;

    VERIFY_PROGRAM_STATE(state);
    LIBERROR_IS_NULL(start_tag);
    LIBERROR_IS_NULL(end_tag);

    for(line_index = 0; line_index < carray_length(state->input_lines); line_index++) {
        struct CString line;

        LIBERROR_OUT_OF_BOUNDS(line_index, carray_length(state->input_lines));
        VERIFY_CSTRING(&(state->input_lines->contents[line_index]));

        line = state->input_lines->contents[line_index];
        common_parse_read_tag(line, &(state->tag_name)); 

        VERIFY_CSTRING(&(state->tag_name));

        /* Start of the tag found-- begin skipping blocks */
        if(strcmp((state)->tag_name.contents, start_tag) == 0) {
            /* If the tags are the same, we want to simply invert the in_multiline variable
             * when a line has the start tag. */
            if(strcmp(start_tag, end_tag) == 0) {
                INVERT_BOOLEAN(in_multiline);
                multiline_tag_line = line_index;

                continue;
            }

            /* If we find the name of the opening tag again when we are supposed
             * to be inside of the start tag already, stop immediately to report
             * an error. */
            if(in_multiline == 1)
                break;

            in_multiline = 1; 
            multiline_tag_line = line_index;

            continue;
        }

        /* End of the tag found-- stop skipping blocks. */
        if(strcmp((state)->tag_name.contents, end_tag) == 0) {
            in_multiline = 0; 

            continue;
        }
    }

    if(in_multiline == 0)
        return;

    fprintf(LIBERROR_STREAM, PROGRAM_NAME ": tag '%s' on line %i not closed\n", start_tag, multiline_tag_line + 1);
    exit(EXIT_UNCLOSED_TAG);
}

void error_fields_have_text(struct ProgramState *state) {
    int line_index = 0;
    int in_multiline = 0;

    VERIFY_PROGRAM_STATE(state);

    for(line_index = 0; line_index < state->input_lines->length; line_index++) {
        struct CString line;
        const char *at_sign = NULL;
        const char *colon_sign = NULL;

        LIBERROR_OUT_OF_BOUNDS(line_index, carray_length(state->input_lines));
        VERIFY_CSTRING(&(state->input_lines->contents[line_index]));
        LIBERROR_IS_NULL(strchr(state->input_lines->contents[line_index].contents, '@'));

        line = state->input_lines->contents[line_index];
        at_sign = strchr(line.contents, '@');
        colon_sign = strchr(at_sign, ':');
        common_parse_read_tag(line, &(state->tag_name)); 

        VERIFY_CSTRING(&(state->tag_name));

        /* Skip the multiline tags */
        if(is_multiline(state->tag_name) == 1) {
            INVERT_BOOLEAN(in_multiline);

            continue; 
        }

        /* Skip group tags */
        if(is_group(state->tag_name) == 1)
            continue; 

        if(in_multiline == 1)
            continue;

        /* No text, basically just a blank '\d+:@' */
        if(state->tag_name.length == 1) {
            fprintf(LIBERROR_STREAM, PROGRAM_NAME ": expected name of tag on line %i, got nothing\n", line_index + 1); 
            exit(EXIT_EXPECTED_TEXT);
        }

        /* We have the name of the tag (and we assume its valid, since this should
         * be ran after all tags have been checked), but is there a ':'? */
        if(CHAR_OFFSET(line.contents, at_sign + state->tag_name.length) >= line.length) {
            fprintf(LIBERROR_STREAM, PROGRAM_NAME ": line %i expected ':' after tag name, got end of line\n", line_index + 1); 
            exit(EXIT_EXPECTED_COLON);
        }

        if(*(at_sign + state->tag_name.length) != ':') {
            fprintf(LIBERROR_STREAM, PROGRAM_NAME ": line %i expected ':' after tag name, got '%c'\n", line_index + 1, *(at_sign + state->tag_name.length)); 
            exit(EXIT_EXPECTED_COLON);
        }

        /* Is there any text after the ':'? */
        if(CHAR_OFFSET(line.contents, colon_sign + 1) >= line.length) {
            fprintf(LIBERROR_STREAM, PROGRAM_NAME ": line %i expected space after colon got the end of the line\n", line_index + 1);
            exit(EXIT_EXPECTED_COLON);
        }

        /* The first character must be a space */
        if(isspace((*(colon_sign + 1))) == 0) {
            fprintf(LIBERROR_STREAM, PROGRAM_NAME ": line %i expected space after colon got '%c'\n", line_index + 1, (*(colon_sign + 1)));
            exit(EXIT_EXPECTED_SPACE);
        }
    }
}

void error_tag_outside_of_docgen_pair(struct ProgramState *state) {
    int line_index = 0;
    int in_docgen_tag = 0;

    VERIFY_PROGRAM_STATE(state);

    for(line_index = 0; line_index < carray_length(state->input_lines); line_index++) {
        struct CString line;

        LIBERROR_OUT_OF_BOUNDS(line_index, carray_length(state->input_lines));
        VERIFY_CSTRING(&(state->input_lines->contents[line_index]));
        LIBERROR_IS_NULL(strchr((state->input_lines->contents[line_index].contents), '@'));

        line = state->input_lines->contents[line_index];
        common_parse_read_tag(line, &(state->tag_name)); 

        VERIFY_CSTRING(&state->tag_name);

        if(strcmp(state->tag_name.contents, DOCGEN_START) == 0) {
            in_docgen_tag = 1;

            continue; 
        }

        if(strcmp(state->tag_name.contents, DOCGEN_END) == 0) {
            in_docgen_tag = 0; 

            continue;
        }

        /* If the next teration after a docgen tag is not inside of atag,
         * then we have a problem */
        if(in_docgen_tag == 1)
            continue;

        fprintf(LIBERROR_STREAM, PROGRAM_NAME ": tag '%s' on line %i outside of pair of docgen tags\n", state->tag_name.contents, line_index + 1);
        exit(EXIT_TAG_OUTSIDE_OF_GROUP);
    }
}

void check_postrequisites(struct ProgramState *state, int index, const char *tag, int postrequisite_count, va_list postrequisites) {
    int line_index = 0;
    int current_postrequisite = 0;
    const char *next_tag = NULL;

    VERIFY_PROGRAM_STATE(state);
    LIBERROR_IS_NULL(tag);
    LIBERROR_IS_NULL(postrequisites);
    LIBERROR_IS_NEGATIVE(index);
    LIBERROR_OUT_OF_BOUNDS(index, carray_length(state->input_lines));
    LIBERROR_IS_VALUE(postrequisite_count, 0);
    
    /* First line is always the tag it was called on-- we do not need
     * to bother with it. */
    index++;

    for(line_index = index; line_index < carray_length(state->input_lines); line_index++) {
        struct CString line;

        /* We have verified all the tags we need to */
        if(current_postrequisite == postrequisite_count)
            return;

        LIBERROR_OUT_OF_BOUNDS(line_index, carray_length(state->input_lines));
        VERIFY_CSTRING(&(state->input_lines->contents[line_index]));
        LIBERROR_IS_NULL(strchr((state->input_lines->contents[line_index].contents), '@'));

        line = state->input_lines->contents[line_index];
        next_tag = va_arg(postrequisites, char *);

        common_parse_read_tag(line, &(state->tag_name)); 
        current_postrequisite++;

        LIBERROR_IS_NULL(next_tag);

        /* Tag is what we expect it to be. Continue on */
        if(strcmp(state->tag_name.contents, next_tag) == 0)
            continue;

        fprintf(LIBERROR_STREAM, PROGRAM_NAME ": expected tag '%s' to follow tag '%s' on line %i, got '%s'\n", next_tag, tag, index + 1, state->tag_name.contents);
        exit(EXIT_EXPECTED_TAG);
    }

    /* Loop ended without all the postrequisites being checked */
    if(current_postrequisite == postrequisite_count)
        return;

    next_tag = va_arg(postrequisites, char *);
    
    LIBERROR_IS_NULL(next_tag);

    fprintf(LIBERROR_STREAM, PROGRAM_NAME ": expected tag '%s' to follow tag '%s' on line %i\n", next_tag, tag, index + 1);
    exit(EXIT_EXPECTED_TAG);
}

void error_tags_have_postrequisites(struct ProgramState *state, const char *tag, int postrequisites, ...) {
    int line_index = 0;
    int in_multiline = 0;

    VERIFY_PROGRAM_STATE(state);
    LIBERROR_IS_NULL(tag);
    LIBERROR_IS_NEGATIVE(postrequisites);
    LIBERROR_IS_VALUE(postrequisites, 0);

    for(line_index = 0; line_index < carray_length(state->input_lines); line_index++) {
        struct CString line;
        va_list required_tags;
    
        LIBERROR_OUT_OF_BOUNDS(line_index, carray_length(state->input_lines));
        VERIFY_CSTRING(&(state->input_lines->contents[line_index]));
        LIBERROR_IS_NULL(strchr((state->input_lines->contents[line_index].contents), '@'));
        LIBERROR_INIT(required_tags);

        line = state->input_lines->contents[line_index];
        common_parse_read_tag(line, &(state->tag_name)); 

        VERIFY_CSTRING(&state->tag_name);

        /* Skip the multiline tags */
        if(is_multiline(state->tag_name) == 1) {
            INVERT_BOOLEAN(in_multiline);

            continue;
        }

        if(in_multiline == 1)
            continue;

        /* This tag is not what we are looking for */
        if(strcmp(state->tag_name.contents, tag) != 0)
            continue;

        va_start(required_tags, postrequisites);
        check_postrequisites(state, line_index, tag, postrequisites, required_tags);
        va_end(required_tags);
    }
}

/* 
 * =========================================
 *          Scanning and Compilation
 * =========================================
*/
void compile_inclusion(struct ProgramState *state, int start_index) {
    int line_index = 0;

    VERIFY_PROGRAM_STATE(state);

    for(line_index = start_index; line_index < carray_length(state->input_lines); line_index++) {
        struct CString line;
    
        LIBERROR_OUT_OF_BOUNDS(line_index, carray_length(state->input_lines));
        VERIFY_CSTRING(&(state->input_lines->contents[line_index]));
        LIBERROR_IS_NULL(strchr((state->input_lines->contents[line_index].contents), '@'));

        line = state->input_lines->contents[line_index];
        common_parse_read_tag(line, &(state->tag_name)); 

        VERIFY_CSTRING(&(state->tag_name));

        /* Do not go past the end of the docgen block! */
        if(strcmp(state->tag_name.contents, DOCGEN_END) == 0)
            break;

        if(strcmp(state->tag_name.contents, "@include") != 0)
            continue;

        fprintf(state->compilation_output, "%s", "START_PREPEND_TO SYNOPSIS\n"); 
        fprintf(state->compilation_output, "#include \"%s\"\n", strchr(line.contents, ' ') + 1); 
        fprintf(state->compilation_output, "%s", "END_PREPEND_TO\n"); 
    }
}

void compile_multilines(struct ProgramState *state, int start_index) {
    int line_index = 0;
    int in_multiline = 0;

    for(line_index = start_index; line_index < carray_length(state->input_lines); line_index++) {
        struct CString line;
    
        LIBERROR_OUT_OF_BOUNDS(line_index, carray_length(state->input_lines));
        VERIFY_CSTRING(&(state->input_lines->contents[line_index]));
        LIBERROR_IS_NULL(strchr((state->input_lines->contents[line_index].contents), '@'));

        line = state->input_lines->contents[line_index];
        common_parse_read_tag(line, &(state->tag_name));

        VERIFY_CSTRING(&(state->tag_name));

        /* Do not go past the end of the docgen block! */
        if(strcmp(state->tag_name.contents, DOCGEN_END) == 0)
            break;

        /* Display the start or end marker of this section */
        if(is_multiline(state->tag_name) == 1) {
            INVERT_BOOLEAN(in_multiline);

            if(in_multiline == 1) {
                fprintf(state->compilation_output, "%s", "START_SECTION "); 

                LIBERROR_IS_NEGATIVE((int) (strlen(state->tag_name.contents + 1)));
                /* Display the section name in all upppercase. (+1 to go past the initial '@'). For
                 * what its worth, we cast here due to the length of the tag's name NEVER going 
                 * above the size of an integer. We have a check above if it ever does when casted,
                 * just in case. */
                common_parse_upper_string(state->compilation_output, state->tag_name.contents + 1, strlen(state->tag_name.contents + 1));

                fprintf(state->compilation_output, "%c", '\n');
            } else if(in_multiline == 0) {
                fprintf(state->compilation_output, "%s", "END_SECTION\n"); 
            } else {
                fprintf(LIBERROR_STREAM, "unimplemented (%s:%i)\n", __FILE__, __LINE__);
                exit(1); 
            }

            continue;
        }

        if(in_multiline == 0)
            continue;

        fprintf(state->compilation_output, "%s\n", strchr(line.contents, '@') + 1);
    }
}

void compile_embed_requests(struct ProgramState *state, int start_index) {
    int line_index = 0;

    VERIFY_PROGRAM_STATE(state);

    for(line_index = start_index; line_index < carray_length(state->input_lines); line_index++) {
        struct CString line;
    
        LIBERROR_OUT_OF_BOUNDS(line_index, carray_length(state->input_lines));
        VERIFY_CSTRING(&(state->input_lines->contents[line_index]));
        LIBERROR_IS_NULL(strchr((state->input_lines->contents[line_index].contents), '@'));

        line = state->input_lines->contents[line_index];
        common_parse_read_tag(line, &(state->tag_name)); 

        VERIFY_CSTRING(&(state->tag_name));

        /* Do not go past the end of the docgen block! */
        if(strcmp(state->tag_name.contents, DOCGEN_END) == 0)
            break;

        if(strcmp(state->tag_name.contents, "@embed") != 0)
            continue;

        fprintf(state->compilation_output, "%s ", "START_EMBED_REQUEST"); 

        /* Get the name of the thing to embed */
        line = state->input_lines->contents[line_index];
        common_parse_read_tag(line, &(state->tag_name)); 
        fprintf(state->compilation_output, "%s\n", strchr(line.contents, ' ') + 1);

        /* Determine whether or not briefs are shown */

        line = state->input_lines->contents[line_index + 1];
        common_parse_read_tag(line, &(state->tag_name)); 
        fprintf(state->compilation_output, "%s\n", strchr(line.contents, ' ') + 1);

        fprintf(state->compilation_output, "%s", "END_EMBED_REQUEST\n"); 
    }
}

void compile_errors(struct ProgramState *state, int start_index) {
    int line_index = 0;

    VERIFY_PROGRAM_STATE(state);

    for(line_index = start_index; line_index < carray_length(state->input_lines); line_index++) {
        struct CString line;
 
        LIBERROR_OUT_OF_BOUNDS(line_index, carray_length(state->input_lines));
        VERIFY_CSTRING(&(state->input_lines->contents[line_index]));
        LIBERROR_IS_NULL(strchr((state->input_lines->contents[line_index].contents), '@'));

        line = state->input_lines->contents[line_index];
        common_parse_read_tag(line, &(state->tag_name)); 

        VERIFY_CSTRING(&(state->tag_name));

        /* Do not go past the end of the docgen block! */
        if(strcmp(state->tag_name.contents, DOCGEN_END) == 0)
            break;

        /* Regular error */
        if(strcmp(state->tag_name.contents, "@error") == 0) {
            const char *error_description = strchr(state->input_lines->contents[line_index].contents, ' ') + 1;

            fprintf(state->compilation_output, "%s", "START_APPEND_TO DESCRIPTION\n"); 
            fprintf(state->compilation_output, "%s\n", error_description); 
            fprintf(state->compilation_output, "%s", "END_APPEND_TO\n"); 

            continue;
        }
    }
}

void compile_parameters(struct ProgramState *state, int start_index) {
    int line_index = 0;

    VERIFY_PROGRAM_STATE(state);

    for(line_index = start_index; line_index < carray_length(state->input_lines); line_index++) {
        struct CString line;
    
        LIBERROR_OUT_OF_BOUNDS(line_index, carray_length(state->input_lines));
        VERIFY_CSTRING(&(state->input_lines->contents[line_index]));
        LIBERROR_IS_NULL(strchr((state->input_lines->contents[line_index].contents), '@'));

        line = state->input_lines->contents[line_index];
        common_parse_read_tag(line, &(state->tag_name)); 

        VERIFY_CSTRING(&(state->tag_name));

        /* Do not go past the end of the docgen block! */
        if(strcmp(state->tag_name.contents, DOCGEN_END) == 0)
            break;

        /* Function parameters */
        if(strcmp(state->tag_name.contents, "@fparam") == 0) {
            const char *param_name = strchr(state->input_lines->contents[line_index].contents, ' ') + 1;
            const char *param_brief = strchr(state->input_lines->contents[line_index + 2].contents, ' ') + 1;

            fprintf(state->compilation_output, "%s", "START_APPEND_TO DESCRIPTION\n"); 
            fprintf(state->compilation_output, "%s will be %s\n", param_name, param_brief); 
            fprintf(state->compilation_output, "%s", "END_APPEND_TO\n"); 

            continue;
        }

        /* Macro parameters */
        if(strcmp(state->tag_name.contents, "@mparam") == 0) {
            const char *param_name = strchr(state->input_lines->contents[line_index].contents, ' ') + 1;
            const char *param_brief = strchr(state->input_lines->contents[line_index + 1].contents, ' ') + 1;

            fprintf(state->compilation_output, "%s", "START_APPEND_TO DESCRIPTION\n"); 
            fprintf(state->compilation_output, "%s will be %s\n", param_name, param_brief); 
            fprintf(state->compilation_output, "%s", "END_APPEND_TO\n"); 

            continue;
        }
    }
}

void compile_references(struct ProgramState *state, int start_index) {
    int line_index = 0;

    VERIFY_PROGRAM_STATE(state);

    for(line_index = start_index; line_index < carray_length(state->input_lines); line_index++) {
        struct CString line;
    
        LIBERROR_OUT_OF_BOUNDS(line_index, carray_length(state->input_lines));
        VERIFY_CSTRING(&(state->input_lines->contents[line_index]));
        LIBERROR_IS_NULL(strchr((state->input_lines->contents[line_index].contents), '@'));

        line = state->input_lines->contents[line_index];
        common_parse_read_tag(line, &(state->tag_name)); 

        VERIFY_CSTRING(&(state->tag_name));

        /* Do not go past the end of the docgen block! */
        if(strcmp(state->tag_name.contents, DOCGEN_END) == 0)
            break;

        /* Dump reference tags */
        if(strcmp(state->tag_name.contents, "@reference") == 0) {
            fprintf(state->compilation_output, "%s", "START_REFERENCE\n"); 
            fprintf(state->compilation_output, "%s\n", strchr(strtok(line.contents, "("), ' ') + 1); 
            fprintf(state->compilation_output, "%s\n", strtok(NULL, ")")); 
            fprintf(state->compilation_output, "%s", "END_REFERENCE\n"); 

            continue;
        }
    }
}

void compile_return(struct ProgramState *state, int start_index) {
    int line_index = 0;

    VERIFY_PROGRAM_STATE(state);

    for(line_index = start_index; line_index < carray_length(state->input_lines); line_index++) {
        struct CString line;
    
        LIBERROR_OUT_OF_BOUNDS(line_index, carray_length(state->input_lines));
        VERIFY_CSTRING(&(state->input_lines->contents[line_index]));
        LIBERROR_IS_NULL(strchr((state->input_lines->contents[line_index].contents), '@'));

        line = state->input_lines->contents[line_index];
        common_parse_read_tag(line, &(state->tag_name)); 

        VERIFY_CSTRING(&(state->tag_name));

        /* Do not go past the end of the docgen block! */
        if(strcmp(state->tag_name.contents, DOCGEN_END) == 0)
            break;

        /* Dump the return tag, although in this context, we only care about the brief. */
        if(strcmp(state->tag_name.contents, "@return") == 0) {
            line = state->input_lines->contents[line_index + 1];
            common_parse_read_tag(line, &(state->tag_name)); 

            VERIFY_CSTRING(&(state->tag_name));

            fprintf(state->compilation_output, "%s", "START_SECTION RETURN VALUE\n"); 
            fprintf(state->compilation_output, "%s\n", FIELD_VALUE(line)); 
            fprintf(state->compilation_output, "%s", "END_SECTION\n"); 

            continue;
        }
    }
}

/* 
 * =========================================
 *             Main Function
 * =========================================
*/

int main(void) {
    int line_index = 0;
    struct ProgramState state;

    LIBERROR_INIT(state);

    /* Initialize the program state (mostly for memory re-use */
    state.input_lines = carray_init(state.input_lines, CSTRING);
    state.tag_name = cstring_init("");
    state.compilation_output = stdout;
    state.temp_function.name = cstring_init("");
    state.temp_function.return_type = cstring_init("");
    state.temp_function.return_description  = cstring_init("");
    state.temp_function.description  = cstring_init("");
    state.temp_function.parameters = carray_init(state.temp_function.parameters, FUNCTION_PARAMETER);
    state.temp_macro_function.name = cstring_init("");
    state.temp_macro_function.description  = cstring_init("");
    state.temp_macro_function.parameters = carray_init(state.temp_macro_function.parameters, MACRO_FUNCTION_PARAMETER);

    common_parse_readlines(state.input_lines, stdin);

    error_lines_have_prefix(&state);
    error_all_tags_recognized(&state);
    error_fields_have_text(&state);

    /* Verify all multiline tags are closed */
    error_tag_is_closed(&state, "@description", "@description");
    error_tag_is_closed(&state, "@notes", "@notes");
    error_tag_is_closed(&state, "@examples", "@examples");
    error_tag_is_closed(&state, "@arguments", "@arguments");

    /* Verify all group tags are closed */
    error_tag_is_closed(&state, "@docgen_start", "@docgen_end");

    /* Note: this might end up with nested structures being stopped by the same struct_end.
     * It might be a better idea to count the number of both start and stops, seeing if its
     * balanced, inside of its own function. */
    error_tag_is_closed(&state, "@struct_start", "@struct_end");

    /* Verify all tags have the tags that they require following them */
    error_tags_have_postrequisites(&state, "@mparam",       1, "@brief");
    error_tags_have_postrequisites(&state, "@return",       1, "@brief");
    error_tags_have_postrequisites(&state, "@embed",        1, "@show_brief");
    error_tags_have_postrequisites(&state, "@field",        2, "@type", "@brief");
    error_tags_have_postrequisites(&state, "@struct_start", 2, "@name", "@brief");
    error_tags_have_postrequisites(&state, "@fparam",       2, "@type", "@brief");
    error_tags_have_postrequisites(&state, "@docgen_start", 3, "@type", "@name", "@brief");

    error_tag_outside_of_docgen_pair(&state);

    /* Scan the stdin for occurrences of the start and end of a docgen block, and
     * produce the markers for them. Once the start is found, invoke the various
     * compilation stages, which will stop themselves once they reach the end of
     * the block they are called in. */
    for(line_index = 0; line_index < carray_length(state.input_lines); line_index++) {
        struct CString line;
    
        VERIFY_CARRAY(state.input_lines);
        LIBERROR_OUT_OF_BOUNDS(line_index, carray_length(state.input_lines));
        VERIFY_CSTRING(&(state.input_lines->contents[line_index]));
        LIBERROR_IS_NULL(strchr((state.input_lines->contents[line_index].contents), '@'));

        line = state.input_lines->contents[line_index];
        common_parse_read_tag(line, &(state.tag_name)); 

        VERIFY_CSTRING(&(state.tag_name));

        /* Note, the lack of 'continue;' here is intentional. The
         * fall through will 'signal' the code below it to start
         * performing compilation, as if the tag is not a start or
         * end tag, it will be ignored, so the only case where
         * the tag will not be ignored is when its the start tag. */
        if(strcmp(state.tag_name.contents, DOCGEN_START) == 0) {
            fprintf(state.compilation_output, "START_GROUP %s\n", strchr(state.input_lines->contents[line_index + 2].contents, ' ') + 1);
        } else if(strcmp(state.tag_name.contents, DOCGEN_END) == 0) {
            fprintf(state.compilation_output, "%s", "END_GROUP\n");

            continue;
        } else {
            continue;
        }

        /* Generate some of the other sections */
        fprintf(state.compilation_output, "%s", "START_SECTION NAME\n");
        fprintf(state.compilation_output, "%s - %s\n", strchr(state.input_lines->contents[line_index + 2].contents, ' ') + 1, strchr(state.input_lines->contents[line_index + 3].contents, ' ') + 1);
        fprintf(state.compilation_output, "%s", "END_SECTION\n");


        /* Begin the various compilation phases, where each (except
         * embedding) starts at our current index, and stops when it
         * reaches the end of the block.  */
        compile_inclusion(&state, line_index);
        compile_multilines(&state, line_index);
        compile_embed_requests(&state, line_index);
        compile_return(&state, line_index);

        /* If there is text in the description AND we have errors to write,
         * they need an empty line in between */
        if(has_description(&state, line_index) == 1 && has_errors(&state, line_index) == 1) {
            fprintf(state.compilation_output, "%s", "START_APPEND_TO DESCRIPTION\n");
            fprintf(state.compilation_output, "%s", "\n\n");
            fprintf(state.compilation_output, "%s", "END_APPEND_TO\n");
        }

        /* Add pre-text to the error list */
        if(has_errors(&state, line_index) == 1) {
            fprintf(state.compilation_output, "%s", "START_APPEND_TO DESCRIPTION\n");
            fprintf(state.compilation_output, "%s", "When the following conditions are met, this will produce"
                                                     "  an error message to stderr, and abort the program.\n"); 
            fprintf(state.compilation_output, "%s", "END_APPEND_TO\n");
        }

        compile_errors(&state, line_index);

        /* If there is errors AND parameters, we need an extra newline
         * between the two */
        if((has_errors(&state, line_index) == 1) && (has_parameters(&state, line_index) == 1)) {
            fprintf(state.compilation_output, "%s", "START_APPEND_TO DESCRIPTION\n");
            fprintf(state.compilation_output, "%s", "\n");
            fprintf(state.compilation_output, "%s", "END_APPEND_TO\n");
        }

        compile_parameters(&state, line_index);
        compile_references(&state, line_index);

        /* Functions and macro functions implicitly embed themselves with no brief showed */
        if(strcmp(strchr(state.input_lines->contents[line_index + 1].contents, ' ') + 1, "function") == 0 ||
           strcmp(strchr(state.input_lines->contents[line_index + 1].contents, ' ') + 1, "macro_function") == 0) {
             
            fprintf(state.compilation_output, "%s", "START_EMBED_REQUEST ");
            fprintf(state.compilation_output, "%s\n", strchr(state.input_lines->contents[line_index + 2].contents, ' ') + 1);
            fprintf(state.compilation_output, "%i\n", 0);
            fprintf(state.compilation_output, "%s", "END_EMBED_REQUEST\n");
         }
    }

    /* Compile all the embeds. This happens agnostic of the line index. */
    compile_function_embeds(&state);
    compile_structure_embeds(&state);
    compile_macro_function_embeds(&state);
    compile_constant_embeds(&state);

    /* Cleanup */
    carray_free(state.input_lines, CSTRING);
    carray_free(state.temp_function.parameters, FUNCTION_PARAMETER);
    carray_free(state.temp_macro_function.parameters, MACRO_FUNCTION_PARAMETER);

    cstring_free(state.tag_name);
    cstring_free(state.temp_function.description);
    cstring_free(state.temp_function.name);
    cstring_free(state.temp_function.return_type);
    cstring_free(state.temp_function.return_description);
    cstring_free(state.temp_macro_function.description);
    cstring_free(state.temp_macro_function.name);

    return EXIT_SUCCESS;
}
