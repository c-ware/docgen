# This Makefile is the output of a template Makefile that was
# processed by the m4 macro preprocessor. It is not intended
# to be human readable.

# Makefile for Watcom (and OpenWatcom) C/C++

CC=wcc386
LINK=wlink
CFLAGS=-bt=nt
OBJS=syscmd(`find src/ -type f -name "*.c" | tr "\n" " " | sed "s/\.c/.obj/g" | sed "s://:/:g" | sed "s:/:\\\:g"')
OBJS_LINK=syscmd(`find src/ -type f -name "*.c" | tr "\n" " " | sed "s/\.c/.obj/g" | sed "s/ / file /g" | sed "s://:/:g" | sed "s:/:\\\:g" | sed "s/^/file /g"')

.c.obj:
	$(CC) $(CFLAGS) $< /fo=$@

all: $(OBJS) docgen.exe

docgen: $(OBJS)
	$(LINK) name docgen.exe $(OBJS_LINK)

clean:
	rm -f $(OBJS)
	rm -f docgen
