dnl Influential variables:
dnl CC          the compiler binary
dnl LD          the linker binary
dnl OS          the OS running on (os2, os2v2, dos, nt)

dnl Signal that we intend to be ran on DOS, OS/2, or Windows.

define(`M4KE_DOS', `')

include(m4ke.m4)

CC=wcc386
LD=wlink
OBJS=CONVERT_FILES(src, .c, .obj)
BINS=CONVERT_FILES(src, .c, .exe, main\.c, 1)
DEPS=CONVERT_FILES_TO_COMMA(src, .c, .obj, main\.c)

all: $(OBJS) $(BINS)

clean: .SYMBOLIC
	for %f in ($(OBJS)) do del %f
	for %f in ($(BINS)) do del %f

.SUFFIXES:

dnl Declare all implicit rules
NEW_IMPLICIT_RULE(.c, .obj, `	$(CC) $1 -fo=$2')
NEW_IMPLICIT_RULE(.obj, .exe, `	$(LD) FILE $1,$(DEPS) NAME $2')

dnl Build all the base objects
NEW_RULE(src\compilers\compiler-c\main, .c, .obj)
NEW_RULE(src\compilers\compiler-c\embeds\structures, .c, .obj)
NEW_RULE(src\compilers\compiler-c\embeds\functions, .c, .obj)
NEW_RULE(src\compilers\compiler-c\embeds\macro_functions, .c, .obj)
NEW_RULE(src\compilers\compiler-c\embeds\constants, .c, .obj)
NEW_RULE(src\compilers\compiler-m4\main, .c, .obj)
NEW_RULE(src\compilers\compiler-m4\embeds\macro_functions, .c, .obj)
NEW_RULE(src\backends\manpage\main, .c, .obj)
NEW_RULE(src\common\errors\errors, .c, .obj)
NEW_RULE(src\common\parsing\parsing, .c, .obj)
NEW_RULE(src\extractors\extractor-c\main, .c, .obj)
NEW_RULE(src\extractors\extractor-m4\main, .c, .obj)
NEW_RULE(src\deps\cstring\cstring, .c, .obj)
NEW_RULE(src\deps\argparse\argparse, .c, .obj)
NEW_RULE(src\deps\argparse\extract, .c, .obj)
NEW_RULE(src\deps\argparse\ap_inter, .c, .obj)

dnl Build the final binaries, which rely on the dependencies
NEW_RULE(src\extractors\extractor-c\main, .obj, .exe)
NEW_RULE(src\extractors\extractor-m4\main, .obj, .exe)
NEW_RULE(src\compilers\compiler-c\main, .obj, .exe)
NEW_RULE(src\compilers\compiler-m4\main, .obj, .exe)
NEW_RULE(src\backends\manpage\main, .obj, .exe)
