CC=cc
PREFIX=/usr/local
OBJS=../../deps/cstring/cstring.o ../../common/errors/errors.o ../../common/parsing/parsing.o ../../deps/argparse/ap_inter.o ../../deps/argparse/argparse.o ../../deps/argparse/extract.o
CFLAGS=-Wall -Wextra -Wshadow -g -ansi -Wno-unused-variable -Wno-unused-parameter
PROGNAME=docgen-backend-manpage

all: $(OBJS) $(PROGNAME)

clean:
	rm -f $(OBJS)
	rm -f $(PROGNAME)

install:
	mkdir -p $(PREIFX)/bin
	cp $(PROGNAME) $(PREFIX)/bin

$(PROGNAME): main.c $(OBJS)
	$(CC) main.c $(OBJS) -o $@ $(CFLAGS)

../../deps/cstring/cstring.o: ../../deps/cstring/cstring.c
	$(CC) $< -o $@ -c $(CFLAGS)

../../common/errors/errors.o: ../../common/errors/errors.c
	$(CC) $< -o $@ -c $(CFLAGS)

../../common/parsing/parsing.o: ../../common/parsing/parsing.c
	$(CC) $< -o $@ -c $(CFLAGS)

../../deps/argparse/ap_inter.o: ../../deps/argparse/ap_inter.c
	$(CC) $< -o $@ -c $(CFLAGS)

../../deps/argparse/argparse.o: ../../deps/argparse/argparse.c
	$(CC) $< -o $@ -c $(CFLAGS)

../../deps/argparse/extract.o: ../../deps/argparse/extract.c
	$(CC) $< -o $@ -c $(CFLAGS)
