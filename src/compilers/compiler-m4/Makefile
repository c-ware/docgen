CC=cc
PREFIX=/usr/local
OBJS=../../deps/cstring/cstring.o ../../common/errors/errors.o ../../common/parsing/parsing.o embeds/macro_functions.o
CFLAGS=-Wall -Wextra -Wshadow -g -ansi -Wno-unused-variable -Wno-unused-parameter
PROGNAME=docgen-compiler-m4

all: $(OBJS) $(PROGNAME)

clean:
	rm -f $(OBJS)
	rm -f $(PROGNAME)

install:
	mkdir -p $(PREFIX)/bin
	cp $(PROGNAME) $(PREFIX)/bin

$(PROGNAME): main.c $(OBJS)
	$(CC) main.c $(OBJS) -o $@ $(CFLAGS)

embeds/macro_functions.o: embeds/macro_functions.c
	$(CC) embeds/macro_functions.c -o $@ -c $(CFLAGS)

../../deps/cstring/cstring.o: ../../deps/cstring/cstring.c
	$(CC) ../../deps/cstring/cstring.c -o $@ -c $(CFLAGS)

../../common/errors/errors.o: ../../common/errors/errors.c
	$(CC) ../../common/errors/errors.c -o $@ -c $(CFLAGS)

../../common/parsing/parsing.o: ../../common/parsing/parsing.c
	$(CC) ../../common/parsing/parsing.c -o $@ -c $(CFLAGS)
