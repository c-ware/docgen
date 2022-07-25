# This Makefile is the output of a template Makefile that was
# processed by the m4 macro preprocessor. It is not intended
# to be human readable.

# Makefile for Watcom (and OpenWatcom) C/C++

CC=wcc386
LINK=wlink
CFLAGS=-bt=nt
OBJS=src\tags\tags.obj src\postprocessors\postprocessors.obj src\libpath\join_path.obj src\libpath\mkdir.obj src\libpath\exists.obj src\cstring\cstring.obj src\libmatch\read.obj src\libmatch\match.obj src\libmatch\cond.obj src\libmatch\cursor.obj src\arguments.obj src\libarg\libarg.obj src\generators\common.obj src\generators\generators.obj src\common\common.obj src\common\error.obj src\main.obj src\extractors\macro_functions\macro_functions.obj src\extractors\projects\projects.obj src\extractors\macros\macros.obj src\extractors\functions\functions.obj src\extractors\structures\structures.obj src\libstr\replace.obj src\libstr\starts.obj src\libstr\count.obj src\libstr\strip.obj src\libstr\reverse.obj 
OBJS_LINK=file src\tags\tags.obj file src\postprocessors\postprocessors.obj file src\libpath\join_path.obj file src\libpath\mkdir.obj file src\libpath\exists.obj file src\cstring\cstring.obj file src\libmatch\read.obj file src\libmatch\match.obj file src\libmatch\cond.obj file src\libmatch\cursor.obj file src\arguments.obj file src\libarg\libarg.obj file src\generators\common.obj file src\generators\generators.obj file src\common\common.obj file src\common\error.obj file src\main.obj file src\extractors\macro_functions\macro_functions.obj file src\extractors\projects\projects.obj file src\extractors\macros\macros.obj file src\extractors\functions\functions.obj file src\extractors\structures\structures.obj file src\libstr\replace.obj file src\libstr\starts.obj file src\libstr\count.obj file src\libstr\strip.obj file src\libstr\reverse.obj file 

.c.obj:
	$(CC) $(CFLAGS) $< /fo=$@

all: $(OBJS) docgen.exe

docgen: $(OBJS)
	$(LINK) name docgen.exe $(OBJS_LINK)

clean:
	rm -f $(OBJS)
	rm -f docgen
