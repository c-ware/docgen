all:
	cd src/backends/manpage; make
	cd src/compilers/compiler-c; make
	cd src/extractors/extractor-c; make
	cd src/compilers/compiler-m4; make
	cd src/extractors/extractor-m4; make

install:
	cd src/backends/manpage; make install
	cd src/compilers/compiler-c; make install
	cd src/extractors/extractor-c; make install
	cd src/compilers/compiler-m4; make install
	cd src/extractors/extractor-m4; make install

clean:
	cd src/backends/manpage; make clean
	cd src/compilers/compiler-c; make clean
	cd src/extractors/extractor-c; make clean
	cd src/compilers/compiler-m4; make clean
	cd src/extractors/extractor-m4; make clean
