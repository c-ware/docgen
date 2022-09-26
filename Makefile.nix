















CC=cc
PREFIX=/usr/local
OBJS=src/compilers/compiler-c/main.o src/compilers/compiler-c/embeds/structures.o src/compilers/compiler-c/embeds/functions.o src/compilers/compiler-c/embeds/macro_functions.o src/compilers/compiler-c/embeds/constants.o src/compilers/compiler-m4/main.o src/compilers/compiler-m4/embeds/macro_functions.o src/backends/manpage/main.o src/common/errors/errors.o src/common/parsing/parsing.o src/extractors/extractor-c/main.o src/extractors/extractor-m4/main.o src/deps/cstring/cstring.o src/deps/argparse/argparse.o src/deps/argparse/extract.o src/deps/argparse/ap_inter.o 
BINS=src/compilers/compiler-c/main src/compilers/compiler-m4/main src/backends/manpage/main src/extractors/extractor-c/main src/extractors/extractor-m4/main 
DEPS=src/compilers/compiler-c/embeds/structures.o src/compilers/compiler-c/embeds/functions.o src/compilers/compiler-c/embeds/macro_functions.o src/compilers/compiler-c/embeds/constants.o src/compilers/compiler-m4/embeds/macro_functions.o src/common/errors/errors.o src/common/parsing/parsing.o src/deps/cstring/cstring.o src/deps/argparse/argparse.o src/deps/argparse/extract.o src/deps/argparse/ap_inter.o 

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


	


	


src/compilers/compiler-c/main.o: src/compilers/compiler-c/main.c 
	$(CC) -c src/compilers/compiler-c/main.c -o src/compilers/compiler-c/main.o
src/compilers/compiler-c/embeds/structures.o: src/compilers/compiler-c/embeds/structures.c 
	$(CC) -c src/compilers/compiler-c/embeds/structures.c -o src/compilers/compiler-c/embeds/structures.o
src/compilers/compiler-c/embeds/functions.o: src/compilers/compiler-c/embeds/functions.c 
	$(CC) -c src/compilers/compiler-c/embeds/functions.c -o src/compilers/compiler-c/embeds/functions.o
src/compilers/compiler-c/embeds/macro_functions.o: src/compilers/compiler-c/embeds/macro_functions.c 
	$(CC) -c src/compilers/compiler-c/embeds/macro_functions.c -o src/compilers/compiler-c/embeds/macro_functions.o
src/compilers/compiler-c/embeds/constants.o: src/compilers/compiler-c/embeds/constants.c 
	$(CC) -c src/compilers/compiler-c/embeds/constants.c -o src/compilers/compiler-c/embeds/constants.o
src/compilers/compiler-m4/main.o: src/compilers/compiler-m4/main.c 
	$(CC) -c src/compilers/compiler-m4/main.c -o src/compilers/compiler-m4/main.o
src/compilers/compiler-m4/embeds/macro_functions.o: src/compilers/compiler-m4/embeds/macro_functions.c 
	$(CC) -c src/compilers/compiler-m4/embeds/macro_functions.c -o src/compilers/compiler-m4/embeds/macro_functions.o
src/backends/manpage/main.o: src/backends/manpage/main.c 
	$(CC) -c src/backends/manpage/main.c -o src/backends/manpage/main.o
src/common/errors/errors.o: src/common/errors/errors.c 
	$(CC) -c src/common/errors/errors.c -o src/common/errors/errors.o
src/common/parsing/parsing.o: src/common/parsing/parsing.c 
	$(CC) -c src/common/parsing/parsing.c -o src/common/parsing/parsing.o
src/extractors/extractor-c/main.o: src/extractors/extractor-c/main.c 
	$(CC) -c src/extractors/extractor-c/main.c -o src/extractors/extractor-c/main.o
src/extractors/extractor-m4/main.o: src/extractors/extractor-m4/main.c 
	$(CC) -c src/extractors/extractor-m4/main.c -o src/extractors/extractor-m4/main.o
src/deps/cstring/cstring.o: src/deps/cstring/cstring.c 
	$(CC) -c src/deps/cstring/cstring.c -o src/deps/cstring/cstring.o
src/deps/argparse/argparse.o: src/deps/argparse/argparse.c 
	$(CC) -c src/deps/argparse/argparse.c -o src/deps/argparse/argparse.o
src/deps/argparse/extract.o: src/deps/argparse/extract.c 
	$(CC) -c src/deps/argparse/extract.c -o src/deps/argparse/extract.o
src/deps/argparse/ap_inter.o: src/deps/argparse/ap_inter.c 
	$(CC) -c src/deps/argparse/ap_inter.c -o src/deps/argparse/ap_inter.o

src/extractors/extractor-c/main: src/extractors/extractor-c/main.o 
	$(CC) src/extractors/extractor-c/main.o $(DEPS) -o src/extractors/extractor-c/main
src/extractors/extractor-m4/main: src/extractors/extractor-m4/main.o 
	$(CC) src/extractors/extractor-m4/main.o $(DEPS) -o src/extractors/extractor-m4/main
src/compilers/compiler-c/main: src/compilers/compiler-c/main.o 
	$(CC) src/compilers/compiler-c/main.o $(DEPS) -o src/compilers/compiler-c/main
src/compilers/compiler-m4/main: src/compilers/compiler-m4/main.o 
	$(CC) src/compilers/compiler-m4/main.o $(DEPS) -o src/compilers/compiler-m4/main
src/backends/manpage/main: src/backends/manpage/main.o 
	$(CC) src/backends/manpage/main.o $(DEPS) -o src/backends/manpage/main

.PHONY: all clean install
