dnl Influential variables:
dnl CC          the compiler binary

include(m4ke.m4)

CC=cc
PREFIX=/usr/local
OBJS=CONVERT_FILES(src, .c, .o)
BINS=CONVERT_FILES(src, .c,, main\.c, 1)
DEPS=CONVERT_FILES(src, .c, .o, main\.c)

all: $(OBJS) $(BINS)

clean:
	rm -f $(OBJS)
	rm -f $(BINS)

install:
	cp src/backends/manpage/main $(PREFIX)/bin
	cp src/compilers/compiler-c/main $(PREFIX)/bin
	cp src/compilers/compiler-m4/main $(PREFIX)/bin
	cp src/extractors/extractor-c/main $(PREFIX)/bin
	cp src/extractors/extractor-m4/main $(PREFIX)/bin

.SUFFIXES:

dnl Declare all implicit rules
NEW_IMPLICIT_RULE(.c, .o, `	$(CC) -c $1 -o $2')
NEW_IMPLICIT_RULE(.o,, `	$(CC) $1 $(DEPS) -o $2')

dnl Build all the base objects
NEW_RULE(src/compilers/compiler-c/main, .c, .o)
NEW_RULE(src/compilers/compiler-c/embeds/structures, .c, .o)
NEW_RULE(src/compilers/compiler-c/embeds/functions, .c, .o)
NEW_RULE(src/compilers/compiler-c/embeds/macro_functions, .c, .o)
NEW_RULE(src/compilers/compiler-c/embeds/constants, .c, .o)
NEW_RULE(src/compilers/compiler-m4/main, .c, .o)
NEW_RULE(src/compilers/compiler-m4/embeds/macro_functions, .c, .o)
NEW_RULE(src/backends/manpage/main, .c, .o)
NEW_RULE(src/common/errors/errors, .c, .o)
NEW_RULE(src/common/parsing/parsing, .c, .o)
NEW_RULE(src/extractors/extractor-c/main, .c, .o)
NEW_RULE(src/extractors/extractor-m4/main, .c, .o)
NEW_RULE(src/deps/cstring/cstring, .c, .o)
NEW_RULE(src/deps/argparse/argparse, .c, .o)
NEW_RULE(src/deps/argparse/extract, .c, .o)
NEW_RULE(src/deps/argparse/ap_inter, .c, .o)

dnl Build the final binaries, which rely on the dependencies
NEW_RULE(src/extractors/extractor-c/main, .o, )
NEW_RULE(src/extractors/extractor-m4/main, .o, )
NEW_RULE(src/compilers/compiler-c/main, .o, )
NEW_RULE(src/compilers/compiler-m4/main, .o, )
NEW_RULE(src/backends/manpage/main, .o, )

.PHONY: all clean install
