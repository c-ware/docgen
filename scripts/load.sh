#!/bin/sh
# Process all files with m4 and output them to their respective directories.

# Process and generate all manual pages
docgen project ./src/main.c --section cware          \
                            --title 'C-Ware Manuals' \
                            --date "`date +'%b %d, %Y'`"

docgen project ./pages/docgen-intro --section cware          \
                                    --title 'C-Ware Manuals' \
                                    --date "`date +'%b %d, %Y'`"

# Process the Makefiles
makegen project unix --binary docgen --main src/main.c > Makefile
makegen project unix --binary docgen --main src/main.c \
                     --cflags '\-Wall -Wextra -Wpedantic -Wshadow -Wno-unused-parameter -ansi -g' > Makefile.dev

m4 ./template/Makefile.mvc > ./Makefile.mvc
m4 ./template/Makefile.wat > ./Makefile.wat
