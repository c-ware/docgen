OBJS=src/arguments.o src/main.o src/libstr/replace.o src/libstr/starts.o src/libstr/count.o src/libstr/strip.o src/libstr/reverse.o src/extractors/structures/structures.o src/extractors/functions/functions.o src/extractors/macros/macros.o src/extractors/projects/projects.o src/extractors/macro_functions/macro_functions.o src/common/common.o src/common/error.o src/generators/common.o src/generators/generators.o src/libarg/libarg.o src/libmatch/read.o src/libmatch/match.o src/libmatch/cond.o src/libmatch/cursor.o src/cstring/cstring.o src/libpath/join_path.o src/libpath/mkdir.o src/libpath/exists.o src/postprocessors/postprocessors.o src/postprocessors/common.o src/postprocessors/writer/writer.o src/tags/tags.o 
TESTOBJS=src/arguments.o src/libstr/replace.o src/libstr/starts.o src/libstr/count.o src/libstr/strip.o src/libstr/reverse.o src/extractors/structures/structures.o src/extractors/functions/functions.o src/extractors/macros/macros.o src/extractors/projects/projects.o src/extractors/macro_functions/macro_functions.o src/common/common.o src/common/error.o src/generators/common.o src/generators/generators.o src/libarg/libarg.o src/libmatch/read.o src/libmatch/match.o src/libmatch/cond.o src/libmatch/cursor.o src/cstring/cstring.o src/libpath/join_path.o src/libpath/mkdir.o src/libpath/exists.o src/postprocessors/postprocessors.o src/postprocessors/common.o src/postprocessors/writer/writer.o src/tags/tags.o 
TESTS=
CC=cc
PREFIX=/usr/local
LDFLAGS=
LDLIBS=
CFLAGS=

all: $(OBJS) $(TESTS) docgen

clean:
	rm -rf $(OBJS)
	rm -rf $(TESTS)
	rm -rf vgcore.*
	rm -rf core*
	rm -rf docgen

install:
	mkdir -p $(PREFIX)
	install -m 755 docgen $(PREFIX)/bin

uninstall:
	rm -f $(PREFIX)/bin/docgen

src/arguments.o: src/arguments.c src/docgen.h
	$(CC) -c $(CFLAGS) src/arguments.c -o src/arguments.o

src/main.o: src/main.c src/docgen.h src/extractors/macros/macros.h src/extractors/projects/projects.h src/extractors/functions/functions.h src/extractors/structures/structures.h src/extractors/macro_functions/macro_functions.h src/generators/generators.h src/postprocessors/writer/writer.h src/postprocessors/postprocessors.h
	$(CC) -c $(CFLAGS) src/main.c -o src/main.o

src/libstr/replace.o: src/libstr/replace.c src/libstr/libstr.h
	$(CC) -c $(CFLAGS) src/libstr/replace.c -o src/libstr/replace.o

src/libstr/starts.o: src/libstr/starts.c src/libstr/libstr.h
	$(CC) -c $(CFLAGS) src/libstr/starts.c -o src/libstr/starts.o

src/libstr/count.o: src/libstr/count.c src/libstr/libstr.h
	$(CC) -c $(CFLAGS) src/libstr/count.c -o src/libstr/count.o

src/libstr/strip.o: src/libstr/strip.c src/libstr/libstr.h
	$(CC) -c $(CFLAGS) src/libstr/strip.c -o src/libstr/strip.o

src/libstr/reverse.o: src/libstr/reverse.c src/libstr/libstr.h
	$(CC) -c $(CFLAGS) src/libstr/reverse.c -o src/libstr/reverse.o

src/extractors/structures/structures.o: src/extractors/structures/structures.c src/extractors/structures/structures.h
	$(CC) -c $(CFLAGS) src/extractors/structures/structures.c -o src/extractors/structures/structures.o

src/extractors/functions/functions.o: src/extractors/functions/functions.c src/extractors/functions/functions.h
	$(CC) -c $(CFLAGS) src/extractors/functions/functions.c -o src/extractors/functions/functions.o

src/extractors/macros/macros.o: src/extractors/macros/macros.c src/extractors/macros/macros.h
	$(CC) -c $(CFLAGS) src/extractors/macros/macros.c -o src/extractors/macros/macros.o

src/extractors/projects/projects.o: src/extractors/projects/projects.c src/docgen.h src/extractors/macros/macros.h src/extractors/functions/functions.h src/extractors/structures/structures.h src/extractors/macro_functions/macro_functions.h src/extractors/projects/projects.h
	$(CC) -c $(CFLAGS) src/extractors/projects/projects.c -o src/extractors/projects/projects.o

src/extractors/macro_functions/macro_functions.o: src/extractors/macro_functions/macro_functions.c src/extractors/macro_functions/macro_functions.h
	$(CC) -c $(CFLAGS) src/extractors/macro_functions/macro_functions.c -o src/extractors/macro_functions/macro_functions.o

src/common/common.o: src/common/common.c src/docgen.h src/common/common.h src/extractors/structures/structures.h
	$(CC) -c $(CFLAGS) src/common/common.c -o src/common/common.o

src/common/error.o: src/common/error.c src/docgen.h src/common/common.h
	$(CC) -c $(CFLAGS) src/common/error.c -o src/common/error.o

src/generators/common.o: src/generators/common.c src/docgen.h src/extractors/macros/macros.h src/extractors/functions/functions.h src/extractors/structures/structures.h src/extractors/macro_functions/macro_functions.h src/generators/generators.h
	$(CC) -c $(CFLAGS) src/generators/common.c -o src/generators/common.o

src/generators/generators.o: src/generators/generators.c src/docgen.h src/generators/generators.h src/extractors/macros/macros.h src/extractors/projects/projects.h src/extractors/functions/functions.h src/extractors/structures/structures.h src/extractors/macro_functions/macro_functions.h
	$(CC) -c $(CFLAGS) src/generators/generators.c -o src/generators/generators.o

src/libarg/libarg.o: src/libarg/libarg.c src/libarg/libarg.h
	$(CC) -c $(CFLAGS) src/libarg/libarg.c -o src/libarg/libarg.o

src/libmatch/read.o: src/libmatch/read.c src/libmatch/libmatch.h
	$(CC) -c $(CFLAGS) src/libmatch/read.c -o src/libmatch/read.o

src/libmatch/match.o: src/libmatch/match.c src/libmatch/libmatch.h
	$(CC) -c $(CFLAGS) src/libmatch/match.c -o src/libmatch/match.o

src/libmatch/cond.o: src/libmatch/cond.c src/libmatch/libmatch.h
	$(CC) -c $(CFLAGS) src/libmatch/cond.c -o src/libmatch/cond.o

src/libmatch/cursor.o: src/libmatch/cursor.c src/libmatch/libmatch.h
	$(CC) -c $(CFLAGS) src/libmatch/cursor.c -o src/libmatch/cursor.o

src/cstring/cstring.o: src/cstring/cstring.c src/cstring/cstring.h
	$(CC) -c $(CFLAGS) src/cstring/cstring.c -o src/cstring/cstring.o

src/libpath/join_path.o: src/libpath/join_path.c src/libpath/libpath.h
	$(CC) -c $(CFLAGS) src/libpath/join_path.c -o src/libpath/join_path.o

src/libpath/mkdir.o: src/libpath/mkdir.c src/libpath/libpath.h
	$(CC) -c $(CFLAGS) src/libpath/mkdir.c -o src/libpath/mkdir.o

src/libpath/exists.o: src/libpath/exists.c src/libpath/libpath.h
	$(CC) -c $(CFLAGS) src/libpath/exists.c -o src/libpath/exists.o

src/postprocessors/postprocessors.o: src/postprocessors/postprocessors.c src/docgen.h src/postprocessors/writer/writer.h src/postprocessors/postprocessors.h src/extractors/macros/macros.h src/extractors/functions/functions.h src/extractors/structures/structures.h src/extractors/macro_functions/macro_functions.h
	$(CC) -c $(CFLAGS) src/postprocessors/postprocessors.c -o src/postprocessors/postprocessors.o

src/postprocessors/common.o: src/postprocessors/common.c src/docgen.h src/postprocessors/writer/writer.h
	$(CC) -c $(CFLAGS) src/postprocessors/common.c -o src/postprocessors/common.o

src/postprocessors/writer/writer.o: src/postprocessors/writer/writer.c src/docgen.h src/postprocessors/postprocessors.h src/postprocessors/writer/writer.h
	$(CC) -c $(CFLAGS) src/postprocessors/writer/writer.c -o src/postprocessors/writer/writer.o

src/tags/tags.o: src/tags/tags.c src/tags/tags.h src/docgen.h
	$(CC) -c $(CFLAGS) src/tags/tags.c -o src/tags/tags.o

docgen: $(OBJS)
	$(CC) $(OBJS) -o docgen $(LDFLAGS) $(LDLIBS)
