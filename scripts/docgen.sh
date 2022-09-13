#!/bin/sh

if [ $# -eq 0 ]; then
	echo "docgen: please specify an input file"
	exit
f


docgen-extractor-c < $1 | docgen-compiler | docgen-backend-manpage
