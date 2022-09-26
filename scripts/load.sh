#!/bin/sh
# Process all files with m4 and output them to their respective directories.

# Process the Makefiles
m4 ./template/Makefile.wat -Itemplate > ./Makefile.wat
m4 ./template/Makefile.nix -Itemplate > ./Makefile.nix
