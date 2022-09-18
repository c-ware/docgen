
















CC=wcc386
LD=wlink
OBJS=src\compilers\compiler-c\main.obj src\compilers\compiler-c\embeds\structures.obj src\compilers\compiler-c\embeds\functions.obj src\compilers\compiler-c\embeds\macro_functions.obj src\compilers\compiler-c\embeds\constants.obj src\compilers\compiler-m4\main.obj src\compilers\compiler-m4\embeds\macro_functions.obj src\backends\manpage\main.obj src\common\errors\errors.obj src\common\parsing\parsing.obj src\extractors\extractor-c\main.obj src\extractors\extractor-m4\main.obj src\deps\cstring\cstring.obj src\deps\argparse\argparse.obj src\deps\argparse\extract.obj src\deps\argparse\ap_inter.obj 
BINS=src\compilers\compiler-c\main.exe src\compilers\compiler-m4\main.exe src\backends\manpage\main.exe src\extractors\extractor-c\main.exe src\extractors\extractor-m4\main.exe 
DEPS=src\compilers\compiler-c\embeds\structures.obj,src\compilers\compiler-c\embeds\functions.obj,src\compilers\compiler-c\embeds\macro_functions.obj,src\compilers\compiler-c\embeds\constants.obj,src\compilers\compiler-m4\embeds\macro_functions.obj,src\common\errors\errors.obj,src\common\parsing\parsing.obj,src\deps\cstring\cstring.obj,src\deps\argparse\argparse.obj,src\deps\argparse\extract.obj,src\deps\argparse\ap_inter.obj

all: $(OBJS) $(BINS)

clean: .SYMBOLIC
	for %f in ($(OBJS)) do del %f
	for %f in ($(BINS)) do del %f

.SUFFIXES:


	


	


src\compilers\compiler-c\main.obj: src\compilers\compiler-c\main.c 
	$(CC) src\compilers\compiler-c\main.c -fo=src\compilers\compiler-c\main.obj
src\compilers\compiler-c\embeds\structures.obj: src\compilers\compiler-c\embeds\structures.c 
	$(CC) src\compilers\compiler-c\embeds\structures.c -fo=src\compilers\compiler-c\embeds\structures.obj
src\compilers\compiler-c\embeds\functions.obj: src\compilers\compiler-c\embeds\functions.c 
	$(CC) src\compilers\compiler-c\embeds\functions.c -fo=src\compilers\compiler-c\embeds\functions.obj
src\compilers\compiler-c\embeds\macro_functions.obj: src\compilers\compiler-c\embeds\macro_functions.c 
	$(CC) src\compilers\compiler-c\embeds\macro_functions.c -fo=src\compilers\compiler-c\embeds\macro_functions.obj
src\compilers\compiler-c\embeds\constants.obj: src\compilers\compiler-c\embeds\constants.c 
	$(CC) src\compilers\compiler-c\embeds\constants.c -fo=src\compilers\compiler-c\embeds\constants.obj
src\compilers\compiler-m4\main.obj: src\compilers\compiler-m4\main.c 
	$(CC) src\compilers\compiler-m4\main.c -fo=src\compilers\compiler-m4\main.obj
src\compilers\compiler-m4\embeds\macro_functions.obj: src\compilers\compiler-m4\embeds\macro_functions.c 
	$(CC) src\compilers\compiler-m4\embeds\macro_functions.c -fo=src\compilers\compiler-m4\embeds\macro_functions.obj
src\backends\manpage\main.obj: src\backends\manpage\main.c 
	$(CC) src\backends\manpage\main.c -fo=src\backends\manpage\main.obj
src\common\errors\errors.obj: src\common\errors\errors.c 
	$(CC) src\common\errors\errors.c -fo=src\common\errors\errors.obj
src\common\parsing\parsing.obj: src\common\parsing\parsing.c 
	$(CC) src\common\parsing\parsing.c -fo=src\common\parsing\parsing.obj
src\extractors\extractor-c\main.obj: src\extractors\extractor-c\main.c 
	$(CC) src\extractors\extractor-c\main.c -fo=src\extractors\extractor-c\main.obj
src\extractors\extractor-m4\main.obj: src\extractors\extractor-m4\main.c 
	$(CC) src\extractors\extractor-m4\main.c -fo=src\extractors\extractor-m4\main.obj
src\deps\cstring\cstring.obj: src\deps\cstring\cstring.c 
	$(CC) src\deps\cstring\cstring.c -fo=src\deps\cstring\cstring.obj
src\deps\argparse\argparse.obj: src\deps\argparse\argparse.c 
	$(CC) src\deps\argparse\argparse.c -fo=src\deps\argparse\argparse.obj
src\deps\argparse\extract.obj: src\deps\argparse\extract.c 
	$(CC) src\deps\argparse\extract.c -fo=src\deps\argparse\extract.obj
src\deps\argparse\ap_inter.obj: src\deps\argparse\ap_inter.c 
	$(CC) src\deps\argparse\ap_inter.c -fo=src\deps\argparse\ap_inter.obj

src\extractors\extractor-c\main.exe: src\extractors\extractor-c\main.obj 
	$(LD) FILE src\extractors\extractor-c\main.obj,$(DEPS) NAME src\extractors\extractor-c\main.exe
src\extractors\extractor-m4\main.exe: src\extractors\extractor-m4\main.obj 
	$(LD) FILE src\extractors\extractor-m4\main.obj,$(DEPS) NAME src\extractors\extractor-m4\main.exe
src\compilers\compiler-c\main.exe: src\compilers\compiler-c\main.obj 
	$(LD) FILE src\compilers\compiler-c\main.obj,$(DEPS) NAME src\compilers\compiler-c\main.exe
src\compilers\compiler-m4\main.exe: src\compilers\compiler-m4\main.obj 
	$(LD) FILE src\compilers\compiler-m4\main.obj,$(DEPS) NAME src\compilers\compiler-m4\main.exe
src\backends\manpage\main.exe: src\backends\manpage\main.obj 
	$(LD) FILE src\backends\manpage\main.obj,$(DEPS) NAME src\backends\manpage\main.exe
