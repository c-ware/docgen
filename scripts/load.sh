#!/bin/sh
# Process all files with m4 and output them to their respective directories.

# Process and generate all manual pages
docgen project ./src/main.c --section cware          \
                            --title 'C-Ware Manuals' \
                            --date "`date +'%b %d, %Y'`"

# Process the Makefiles
m4 ./template/Makefile > ./Makefile
m4 ./template/Makefile.mvc > ./Makefile.mvc
m4 ./template/Makefile.wat > ./Makefile.wat
