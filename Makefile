OBJS=./src/arguments.o ./src/main.o ./src/libarg/libarg.o ./src/libmatch/read.o ./src/libmatch/cond.o ./src/libmatch/cursor.o ./src/libmatch/match.o ./src/libstr/starts.o ./src/libstr/replace.o ./src/libstr/strip.o ./src/libstr/reverse.o ./src/libstr/count.o ./src/libpath/exists.o ./src/libpath/mkdir.o ./src/libpath/join_path.o ./src/common/error.o ./src/common/common.o ./src/tags/tags.o ./src/targets/projects/projects.o ./src/targets/projects/backends/manpage.o ./src/targets/projects/backends/helpcms.o ./src/targets/projects/backends/dcfscript.o ./src/targets/macros/macros.o ./src/targets/macros/backends/manpage.o ./src/targets/functions/functions.o ./src/targets/functions/backends/manpage.o ./src/extractors/structures/structures.o ./src/extractors/macros/macros.o ./src/extractors/macro_functions/macro_functions.o ./src/extractors/functions/functions.o 
TESTOBJS=./src/arguments.o ./src/libarg/libarg.o ./src/libmatch/read.o ./src/libmatch/cond.o ./src/libmatch/cursor.o ./src/libmatch/match.o ./src/libstr/starts.o ./src/libstr/replace.o ./src/libstr/strip.o ./src/libstr/reverse.o ./src/libstr/count.o ./src/libpath/exists.o ./src/libpath/mkdir.o ./src/libpath/join_path.o ./src/common/error.o ./src/common/common.o ./src/tags/tags.o ./src/targets/projects/projects.o ./src/targets/projects/backends/manpage.o ./src/targets/projects/backends/helpcms.o ./src/targets/projects/backends/dcfscript.o ./src/targets/macros/macros.o ./src/targets/macros/backends/manpage.o ./src/targets/functions/functions.o ./src/targets/functions/backends/manpage.o ./src/extractors/structures/structures.o ./src/extractors/macros/macros.o ./src/extractors/macro_functions/macro_functions.o ./src/extractors/functions/functions.o 
TESTS=
CC=cc
PREFIX=/usr/local

all: $(OBJS) $(TESTS) docgen

clean:
	rm -rf $(OBJS)
	rm -rf $(TESTS)
	rm -rf vgcore.*
	rm -rf core*
	rm -rf docgen

install:
	mkdir -p $(PREFIX)
	mkdir -p $(PREFIX)/bin
	install -m 755 docgen $(PREFIX)/bin

uninstall:
	rm -f $(PREFIX)/bin/docgen

./src/arguments.o: ./src/arguments.c ./src/docgen.h
	$(CC) -c $(CFLAGS) ./src/arguments.c -o ./src/arguments.o $(LDFLAGS) $(LDLIBS)

./src/main.o: ./src/main.c ./src/docgen.h ./src/targets/macros/macros.h ./src/targets/projects/projects.h ./src/targets/functions/functions.h
	$(CC) -c $(CFLAGS) ./src/main.c -o ./src/main.o $(LDFLAGS) $(LDLIBS)

./src/libarg/libarg.o: ./src/libarg/libarg.c ./src/libarg/libarg.h
	$(CC) -c $(CFLAGS) ./src/libarg/libarg.c -o ./src/libarg/libarg.o $(LDFLAGS) $(LDLIBS)

./src/libmatch/read.o: ./src/libmatch/read.c ./src/libmatch/libmatch.h
	$(CC) -c $(CFLAGS) ./src/libmatch/read.c -o ./src/libmatch/read.o $(LDFLAGS) $(LDLIBS)

./src/libmatch/cond.o: ./src/libmatch/cond.c ./src/libmatch/libmatch.h
	$(CC) -c $(CFLAGS) ./src/libmatch/cond.c -o ./src/libmatch/cond.o $(LDFLAGS) $(LDLIBS)

./src/libmatch/cursor.o: ./src/libmatch/cursor.c ./src/libmatch/libmatch.h
	$(CC) -c $(CFLAGS) ./src/libmatch/cursor.c -o ./src/libmatch/cursor.o $(LDFLAGS) $(LDLIBS)

./src/libmatch/match.o: ./src/libmatch/match.c ./src/libmatch/libmatch.h
	$(CC) -c $(CFLAGS) ./src/libmatch/match.c -o ./src/libmatch/match.o $(LDFLAGS) $(LDLIBS)

./src/libstr/starts.o: ./src/libstr/starts.c ./src/libstr/libstr.h
	$(CC) -c $(CFLAGS) ./src/libstr/starts.c -o ./src/libstr/starts.o $(LDFLAGS) $(LDLIBS)

./src/libstr/replace.o: ./src/libstr/replace.c ./src/libstr/libstr.h
	$(CC) -c $(CFLAGS) ./src/libstr/replace.c -o ./src/libstr/replace.o $(LDFLAGS) $(LDLIBS)

./src/libstr/strip.o: ./src/libstr/strip.c ./src/libstr/libstr.h
	$(CC) -c $(CFLAGS) ./src/libstr/strip.c -o ./src/libstr/strip.o $(LDFLAGS) $(LDLIBS)

./src/libstr/reverse.o: ./src/libstr/reverse.c ./src/libstr/libstr.h
	$(CC) -c $(CFLAGS) ./src/libstr/reverse.c -o ./src/libstr/reverse.o $(LDFLAGS) $(LDLIBS)

./src/libstr/count.o: ./src/libstr/count.c ./src/libstr/libstr.h
	$(CC) -c $(CFLAGS) ./src/libstr/count.c -o ./src/libstr/count.o $(LDFLAGS) $(LDLIBS)

./src/libpath/exists.o: ./src/libpath/exists.c ./src/libpath/libpath.h
	$(CC) -c $(CFLAGS) ./src/libpath/exists.c -o ./src/libpath/exists.o $(LDFLAGS) $(LDLIBS)

./src/libpath/mkdir.o: ./src/libpath/mkdir.c ./src/libpath/libpath.h
	$(CC) -c $(CFLAGS) ./src/libpath/mkdir.c -o ./src/libpath/mkdir.o $(LDFLAGS) $(LDLIBS)

./src/libpath/join_path.o: ./src/libpath/join_path.c ./src/libpath/libpath.h
	$(CC) -c $(CFLAGS) ./src/libpath/join_path.c -o ./src/libpath/join_path.o $(LDFLAGS) $(LDLIBS)

./src/common/error.o: ./src/common/error.c ./src/common/common.h
	$(CC) -c $(CFLAGS) ./src/common/error.c -o ./src/common/error.o $(LDFLAGS) $(LDLIBS)

./src/common/common.o: ./src/common/common.c ./src/docgen.h ./src/common/common.h ./src/extractors/structures/structures.h
	$(CC) -c $(CFLAGS) ./src/common/common.c -o ./src/common/common.o $(LDFLAGS) $(LDLIBS)

./src/tags/tags.o: ./src/tags/tags.c ./src/tags/tags.h ./src/docgen.h
	$(CC) -c $(CFLAGS) ./src/tags/tags.c -o ./src/tags/tags.o $(LDFLAGS) $(LDLIBS)

./src/targets/projects/projects.o: ./src/targets/projects/projects.c ./src/targets/projects/projects.h
	$(CC) -c $(CFLAGS) ./src/targets/projects/projects.c -o ./src/targets/projects/projects.o $(LDFLAGS) $(LDLIBS)

./src/targets/projects/backends/manpage.o: ./src/targets/projects/backends/manpage.c ./src/targets/projects/projects.h
	$(CC) -c $(CFLAGS) ./src/targets/projects/backends/manpage.c -o ./src/targets/projects/backends/manpage.o $(LDFLAGS) $(LDLIBS)

./src/targets/projects/backends/helpcms.o: ./src/targets/projects/backends/helpcms.c ./src/targets/projects/projects.h
	$(CC) -c $(CFLAGS) ./src/targets/projects/backends/helpcms.c -o ./src/targets/projects/backends/helpcms.o $(LDFLAGS) $(LDLIBS)

./src/targets/projects/backends/dcfscript.o: ./src/targets/projects/backends/dcfscript.c ./src/targets/projects/projects.h
	$(CC) -c $(CFLAGS) ./src/targets/projects/backends/dcfscript.c -o ./src/targets/projects/backends/dcfscript.o $(LDFLAGS) $(LDLIBS)

./src/targets/macros/macros.o: ./src/targets/macros/macros.c ./src/targets/macros/macros.h
	$(CC) -c $(CFLAGS) ./src/targets/macros/macros.c -o ./src/targets/macros/macros.o $(LDFLAGS) $(LDLIBS)

./src/targets/macros/backends/manpage.o: ./src/targets/macros/backends/manpage.c ./src/targets/macros/macros.h
	$(CC) -c $(CFLAGS) ./src/targets/macros/backends/manpage.c -o ./src/targets/macros/backends/manpage.o $(LDFLAGS) $(LDLIBS)

./src/targets/functions/functions.o: ./src/targets/functions/functions.c ./src/targets/functions/functions.h
	$(CC) -c $(CFLAGS) ./src/targets/functions/functions.c -o ./src/targets/functions/functions.o $(LDFLAGS) $(LDLIBS)

./src/targets/functions/backends/manpage.o: ./src/targets/functions/backends/manpage.c ./src/targets/functions/functions.h
	$(CC) -c $(CFLAGS) ./src/targets/functions/backends/manpage.c -o ./src/targets/functions/backends/manpage.o $(LDFLAGS) $(LDLIBS)

./src/extractors/structures/structures.o: ./src/extractors/structures/structures.c ./src/extractors/structures/structures.h
	$(CC) -c $(CFLAGS) ./src/extractors/structures/structures.c -o ./src/extractors/structures/structures.o $(LDFLAGS) $(LDLIBS)

./src/extractors/macros/macros.o: ./src/extractors/macros/macros.c ./src/extractors/macros/macros.h
	$(CC) -c $(CFLAGS) ./src/extractors/macros/macros.c -o ./src/extractors/macros/macros.o $(LDFLAGS) $(LDLIBS)

./src/extractors/macro_functions/macro_functions.o: ./src/extractors/macro_functions/macro_functions.c ./src/extractors/macro_functions/macro_functions.h
	$(CC) -c $(CFLAGS) ./src/extractors/macro_functions/macro_functions.c -o ./src/extractors/macro_functions/macro_functions.o $(LDFLAGS) $(LDLIBS)

./src/extractors/functions/functions.o: ./src/extractors/functions/functions.c ./src/extractors/functions/functions.h
	$(CC) -c $(CFLAGS) ./src/extractors/functions/functions.c -o ./src/extractors/functions/functions.o $(LDFLAGS) $(LDLIBS)

docgen: $(OBJS)
	$(CC) $(OBJS) -o docgen $(LDFLAGS) $(LDLIBS)
