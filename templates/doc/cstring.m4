dnl A collection of M4 macros for producing UNIX manuals.
dnl Mostly intended for documenting C programs, C functions, and more.
dnl
dnl +--------------------------------------------------------------------+
dnl HackerSmacker wrote the updated macro for reversing a string, since he
dnl realized that the rev(1) is not a POSIX or SiUS standard.
dnl +--------------------------------------------------------------------+

dnl Configuration
define(`M4_SECTION', `cware')
define(`M4_CATEGORY', `C-Ware Manuals')
define(`M4_AUTHORS', `AUTHORS HERE')

dnl Internal utility macros-- do not modify, but feel free to use

dnl AWK script to REV_STRINGerse a string
changequote(<, >)
define(<REV_STRING>, <awk '{ for(i = length; i != 0; i--) x = (x `substr'($`'0, i, 1)) } { print x; x = "" }'>)
changequote(`, ')

define(`M4_FILENAME', `syscmd(echo -n __file__ | xargs basename    | \
                              tr "[[:lower:]]" "[[:upper:]]" | REV_STRING | \
                              cut -d "." -f 2- | REV_STRING | tr -d "\n")')

define(`M4_FILENAME_ORIGINAL', `syscmd(echo -n __file__ | xargs basename | \
                                       REV_STRING | cut -d "." -f 2- | REV_STRING      | \
                                       tr -d "\n")')

define(`M4_CURRENT_DATE', `syscmd(`date +"%B %d, %Y" | tr -d "\n"')')

dnl Manual Macros-- These are intended for user
define(`M4_HEADER', `.TH "M4_FILENAME" "M4_SECTION" "M4_CURRENT_DATE" \
" " "M4_CATEGORY"')

define(`M4_BOLD', `.BR "$1" "$2"')
define(`M4_REFERENCE', `.BR "$1" "($2)" "$3"')
define(`M4_ITALICS', `.IR "$1" "$2"')
define(`M4_BREAK', `.br')
define(`M4_EMPTY', `')

define(`M4_LOCAL_INCLUSION', `.B "#include ""$1"""
.br')
define(`M4_SYSTEM_INCLUSION', `.B "#include $1"')

dnl 'Name' section macros
define(`M4_NAME_START', `.SH NAME
M4_FILENAME_ORIGINAL - $1')
define(`M4_NAME_END', `')

dnl 'Synopsis' section macros
define(`M4_SYNOPSIS_START', `.SH SYNOPSIS')
define(`M4_SYNOPSIS_FUNC_START', `.BI dnl')
define(`M4_SYNOPSIS_FUNC_TYPE', `"$1" dnl')
define(`M4_SYNOPSIS_FUNC_ARGUMENT', `"$1" dnl')
define(`M4_SYNOPSIS_FUNC_END', `");
.br dnl"')
define(`M4_SYNOPSIS_END', `')

define(`M4_SYNOPSIS_STRUCT_START', `struct $1 {
.RS 0.4i
.br')
define(`M4_SYNOPSIS_STRUCT_END', `.br
.RE
};')
define(`M4_SYNOPSIS_STRUCT_FIELD', `ifelse($1, `', `', ifelse(index($1, `*'), `-1', $1 $2;, $1$2;))
.br
.br')
define(`M4_SYNOPSIS_INLINE_STRUCT_START', `struct {
.br
.RS 0.4i')

define(`M4_SYNOPSIS_INLINE_STRUCT_END', `.br
.RE
} $1;')

define(`M4_SYNOPSIS_MACRO_FUNCTION_START', `.BI "#define $1(" \')
define(`M4_SYNOPSIS_MACRO_FUNCTION_NEXT', `", " \')
define(`M4_SYNOPSIS_MACRO_FUNCTION_PARAM', `"$1" \')
define(`M4_SYNOPSIS_MACRO_FUNCTION_END', `")
.br"')

define(`M4_SYNOPSIS_IFNDEF', `#ifndef $1
.br
#define $1    $2
.br
#endif
.br
')

dnl 'Description' sectiom macros
define(`M4_DESCRIPTION_START', `.SH DESCRIPTION')
define(`M4_DESCRIPTION_END', `')
define(`M4_DESCRIPTION_FUNC_INTRO', `The
M4_BOLD(`M4_FILENAME_ORIGINAL')
function')

dnl 'Return Value' section macros
define(`M4_RETURN_VALUE_START', `.SH RETURN VALUE')
define(`M4_RETURN_VALUE_END', `')
define(`M4_RETURN_VALUE_FUNC_INTRO', `The
M4_BOLD(`M4_FILENAME_ORIGINAL')
function')
define(`M4_RETURN_VALUE_FUNC_NONE', `This function has no return value.')

dnl 'Example' section macros
define(`M4_EXAMPLE_START', `.SH EXAMPLES')
define(`M4_EXAMPLE_BODY', `ifelse(`$1', `', `', `$1
.br
M4_EXAMPLE_BODY(shift($@))')')

define(`M4_EXAMPLE_FUNC_COMMENT', `/*
 * This example demonstrates how to use the M4_FILENAME_ORIGINAL function.
.br
*/
.br
')
define(`M4_EXAMPLE_END', `')

dnl 'See Also' section macros
define(`M4_SEE_ALSO_START', `.SH SEE ALSO')
define(`M4_SEE_ALSO_MAN', `.BR "$1" "($2),"')
define(`M4_SEE_ALSO_MAN_LAST', `.BR "$1" "($2)"')
define(`M4_SEE_ALSO_END', `')

dnl 'Authors' section
define(`M4_AUTHORS_START', `.SH AUTHORS')
define(`M4_AUTHORS_END', `')

dnl Bulleted list macros
define(`M4_BULLETED_LIST_START', `')
define(`M4_BULLETED_LIST_START_ENTRY', `.IP "o" $1i')
define(`M4_BULLETED_LIST_ENTRY', `$1')
define(`M4_BULLETED_LIST_END', `.IP "" 0i')

dnl Table macros
define(`M4_MANUAL_TABLE_START', `.TS
tab(;);
lb l
_ _
lb l.')
define(`M4_FUNCTION_TABLE_START', `.TS
tab(;);
lb l
_ _
lb l.')
define(`M4_MACRO_TABLE_START', `.TS
tab(;);
lb l
_ _
lb l.')
define(`M4_TABLE_ENTRY', `ifelse(`$1', `', `', `$1;M4_TABLE_ENTRY(shift($@))')')
define(`M4_TABLE_END', `.TE')

dnl Parameter list macros
define(`M4_PARAMETER_LIST_START')
define(`M4_PARAMETER', `M4_ITALICS(`$1')
will be $2
.br')
define(`M4_PARAMETER_LIST_END')
define(`M4_PARAMETER_LIST_FUNC_NONE', `This function has no parameters.')

dnl Error list macros
define(`M4_ERROR_LIST_FUNC_NONE', `This function has no error conditions.')
define(`M4_ERROR_LIST_FUNC_INTRO', `This function will error when any of the following conditions are met.')
define(`M4_ERROR_LIST_START', `M4_BULLETED_LIST_START()') 
define(`M4_ERROR_LIST_ENTRIES', `ifelse($1, `', `', `M4_BULLETED_LIST_START_ENTRY(0.4)
M4_BULLETED_LIST_ENTRY($1)
.br
M4_ERROR_LIST_ENTRIES(shift($@))')')
define(`M4_ERROR_LIST_END', `M4_BULLETED_LIST_END()')

dnl Project-specific macros-- less likely to be general purpose. These are
dnl more intended to reduce boilerplate in some areas. Mostly not shipped with
dnl doclib.
define(`M4_INCLUSIONS', `M4_LOCAL_INCLUSION(`cstring.h')
')
