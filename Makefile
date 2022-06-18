# This Makefile is the output of a template Makefile that was
# processed by the m4 macro preprocessor. It is not intended
# to be human readable.

CC=cc
PREFIX=/usr/local
CFLAGS=
OBJS=src/extractors/functions/functions.o src/extractors/macro_functions/macro_functions.o src/extractors/macros/macros.o src/extractors/structures/structures.o src/targets/functions/functions.o src/targets/functions/backends/manpage.o src/targets/macros/macros.o src/targets/macros/backends/manpage.o src/targets/projects/backends/manpage.o src/targets/projects/backends/helpcms.o src/targets/projects/backends/dcfscript.o src/targets/projects/projects.o src/arguments.o src/tags/tags.o src/common/common.o src/libpath/exists.o src/libpath/mkdir.o src/libpath/join_path.o src/libstr/starts.o src/libstr/replace.o src/libstr/strip.o src/libstr/reverse.o src/libstr/count.o src/libmatch/read.o src/libmatch/cond.o src/libmatch/cursor.o src/libmatch/match.o src/libarg/libarg.o src/main.o 
TEST_OBJS=src/extractors/functions/functions.o src/extractors/macro_functions/macro_functions.o src/extractors/macros/macros.o src/extractors/structures/structures.o src/targets/functions/functions.o src/targets/functions/backends/manpage.o src/targets/macros/macros.o src/targets/macros/backends/manpage.o src/targets/projects/backends/manpage.o src/targets/projects/backends/helpcms.o src/targets/projects/backends/dcfscript.o src/targets/projects/projects.o src/arguments.o src/tags/tags.o src/common/common.o src/libpath/exists.o src/libpath/mkdir.o src/libpath/join_path.o src/libstr/starts.o src/libstr/replace.o src/libstr/strip.o src/libstr/reverse.o src/libstr/count.o src/libmatch/read.o src/libmatch/cond.o src/libmatch/cursor.o src/libmatch/match.o src/libarg/libarg.o 
TESTS=
DOCS=./doc/docgen.cware 
MANNAMES=docgen.cware 
DEBUGGER=

all: $(OBJS) $(TESTS) docgen

clean:
	rm -f $(TESTS)
	rm -f $(OBJS)
	rm -f docgen

check:
	./scripts/check.sh $(DEBUGGER)

install:
	for manual in $(MANNAMES); do					  \
		cp doc/$$manual $(PREFIX)/share/man/mancware; \
	done

	mv docgen /usr/local/bin

uninstall:
	for manual in $(MANNAMES); do					 \
		rm -f $(PREFIX)/share/man/mancware/$$manual; \
	done

docgen: $(OBJS)
	$(CC) $(OBJS) $(CFLAGS) -o docgen

.c.out:
	$(CC) $< $(TEST_OBJS) $(CFLAGS) -o $@

.c.o:
	$(CC) -c $< $(CFLAGS) -o $@
