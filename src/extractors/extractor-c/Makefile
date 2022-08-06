CC=cc
OBJS=../../deps/cstring/cstring.o
CFLAGS=-Wall -Wextra -Wshadow -g -ansi
PROGNAME=docgen-extractor-c

all: $(OBJS) $(PROGNAME)

clean:
	rm -f $(OBJS)
	rm -f $(PROGNAME)

$(PROGNAME): main.c $(OBJS)
	$(CC) $< $(OBJS) -o $@

../../deps/cstring/cstring.o: ../../deps/cstring/cstring.c
	$(CC) $< -o $@ -c
