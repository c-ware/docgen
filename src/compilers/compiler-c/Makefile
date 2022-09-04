CC=cc
OBJS=../../deps/cstring/cstring.o ../../common/errors/errors.o ../../common/parsing/parsing.o embeds/functions.o embeds/structures.o embeds/macro_functions.o embeds/constants.o
CFLAGS=-Wall -Wextra -Wshadow -g -ansi -Wno-unused-variable -Wno-unused-parameter
PROGNAME=docgen-compiler-c

all: $(OBJS) $(PROGNAME)

clean:
	rm -f $(OBJS)
	rm -f $(PROGNAME)

$(PROGNAME): main.c $(OBJS)
	$(CC) main.c $(OBJS) -o $@ $(CFLAGS)

embeds/functions.o: embeds/functions.c
	$(CC) $< -o $@ -c $(CFLAGS)

embeds/constants.o: embeds/constants.c
	$(CC) $< -o $@ -c $(CFLAGS)

embeds/macro_functions.o: embeds/macro_functions.c
	$(CC) $< -o $@ -c $(CFLAGS)

embeds/structures.o: embeds/structures.c
	$(CC) $< -o $@ -c $(CFLAGS)

../../deps/cstring/cstring.o: ../../deps/cstring/cstring.c
	$(CC) $< -o $@ -c $(CFLAGS)

../../common/errors/errors.o: ../../common/errors/errors.c
	$(CC) $< -o $@ -c $(CFLAGS)

../../common/parsing/parsing.o: ../../common/parsing/parsing.c
	$(CC) $< -o $@ -c $(CFLAGS)
