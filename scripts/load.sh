#!/bin/sh
# Process all files with m4 and output them to their respective directories.

# Process and generate all manual pages
docgen project ./src/main.c --section cware          \
                            --title 'C-Ware Manuals' \
                            --date "`date +'%b %d, %Y'`"

# Process the Makefiles
m4 ./templates/Makefile.template > ./Makefile
m4 ./templates/Makefile.msvc.template > ./Makefile.msvc
m4 ./templates/Makefile.watcom.template > ./Makefile.watcom
