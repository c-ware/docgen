# This Makefile is the output of a template Makefile that was
# processed by the m4 macro preprocessor. It is not intended
# to be human readable.

CC=cc
PREFIX=/usr/local
CFLAGS=
OBJS=syscmd(`find src/ -type f -name "*.c" | tr "\n" " " | sed "s/\.c/.o/g" | sed "s://:/:g"')
TEST_OBJS=syscmd(`find src/ -type f -name "*.c" | grep -v "main.c" | tr "\n" " " | sed "s/\.c/.o/g" | sed "s://:/:g"')
TESTS=syscmd(`find tests/ -type f -name "*.c" | tr "\n" " " | sed "s/\.c/.out/g"')
DOCS=syscmd(`find . -type f -name "*.cware" | tr "\n" " "')
MANNAMES=syscmd(`for src in $(find . -type f -name "*.cware"); do echo $src | xargs basename; done | tr "\n" " "')
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
