all:
	cd src/backends/manpage; make; cd ../../..
	cd src/compilers/compiler-c; make; cd ../../..
	cd src/extractors/extractor-c; make; cd ../../..
	cd src/compilers/compiler-m4; make; cd ../../..
	cd src/extractors/extractor-m4; make; cd ../../..

install:
	cd src/backends/manpage; make install; cd ../../..
	cd src/compilers/compiler-c; make install; cd ../../..
	cd src/extractors/extractor-c; make install; cd ../../..
	cd src/compilers/compiler-m4; make install; cd ../../..
	cd src/extractors/extractor-m4; make install; cd ../../..

clean:
	cd src/backends/manpage; make clean; cd ../../..
	cd src/compilers/compiler-c; make clean; cd ../../..
	cd src/extractors/extractor-c; make clean; cd ../../..
	cd src/compilers/compiler-m4; make clean; cd ../../..
	cd src/extractors/extractor-m4; make clean; cd ../../..
