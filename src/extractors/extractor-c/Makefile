CC=cc
OBJS=../../deps/cstring/cstring.o ../../common/errors/errors.o ../../common/parsing/parsing.o
PREFIX=/usr/local
CFLAGS=-Wall -Wextra -Wshadow -g -ansi
PROGNAME=docgen-extractor-c

all: $(OBJS) $(PROGNAME)

clean:
	rm -f $(OBJS)
	rm -f $(PROGNAME)

install:
	mkdir -p $(PREIFX)/bin
	cp $(PROGNAME) $(PREFIX)/bin

$(PROGNAME): main.c $(OBJS)
	$(CC) $< $(OBJS) -o $@ $(CFLAGS)

../../deps/cstring/cstring.o: ../../deps/cstring/cstring.c
	$(CC) $< -o $@ -c $(CFLAGS)

../../common/errors/errors.o: ../../common/errors/errors.c
	$(CC) $< -o $@ -c $(CFLAGS)

../../common/parsing/parsing.o: ../../common/parsing/parsing.c
	$(CC) $< -o $@ -c $(CFLAGS)
