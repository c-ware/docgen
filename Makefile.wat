# This Makefile is the output of a template Makefile that was
# processed by the m4 macro preprocessor. It is not intended
# to be human readable.

# Makefile for Watcom (and OpenWatcom) C/C++

CC=wcc386
LINK=wlink
CFLAGS=-bt=nt
OBJS=src\extractors\functions\functions.obj src\extractors\macro_functions\macro_functions.obj src\extractors\macros\macros.obj src\extractors\structures\structures.obj src\targets\functions\functions.obj src\targets\functions\backends\manpage.obj src\targets\projects\backends\manpage.obj src\targets\projects\backends\helpcms.obj src\targets\projects\backends\dcfscript.obj src\targets\projects\projects.obj src\arguments.obj src\tags\tags.obj src\common\common.obj src\libpath\exists.obj src\libpath\mkdir.obj src\libpath\join_path.obj src\libstr\starts.obj src\libstr\replace.obj src\libstr\strip.obj src\libstr\reverse.obj src\libstr\count.obj src\libmatch\read.obj src\libmatch\cond.obj src\libmatch\cursor.obj src\libmatch\match.obj src\libarg\libarg.obj src\main.obj 
OBJS_LINK=file src\extractors\functions\functions.obj file src\extractors\macro_functions\macro_functions.obj file src\extractors\macros\macros.obj file src\extractors\structures\structures.obj file src\targets\functions\functions.obj file src\targets\functions\backends\manpage.obj file src\targets\projects\backends\manpage.obj file src\targets\projects\backends\helpcms.obj file src\targets\projects\backends\dcfscript.obj file src\targets\projects\projects.obj file src\arguments.obj file src\tags\tags.obj file src\common\common.obj file src\libpath\exists.obj file src\libpath\mkdir.obj file src\libpath\join_path.obj file src\libstr\starts.obj file src\libstr\replace.obj file src\libstr\strip.obj file src\libstr\reverse.obj file src\libstr\count.obj file src\libmatch\read.obj file src\libmatch\cond.obj file src\libmatch\cursor.obj file src\libmatch\match.obj file src\libarg\libarg.obj file src\main.obj file 

.c.obj:
	$(CC) $(CFLAGS) $< /fo=$@

all: $(OBJS) docgen.exe

docgen: $(OBJS)
	$(LINK) name docgen.exe $(OBJS_LINK)

clean:
	rm -f $(OBJS)
	rm -f docgen
