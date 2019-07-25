#!/bin/bash
for file in "ccvwrapper.i ccvwrapper.c ccvwrapper.h"; do
	cp $file ./ccv/lib
done
cd ./ccv/lib

swig -python -py3 ccvwrapper.i
clang -fpic -c ccvwrapper.c ccvwrapper_wrap.c -I/home/mzablocki/anaconda3/include/python3.7m -I"/home/mzablocki/ccv/lib"
clang -L"/home/mzablocki/ccv/lib" -L/home/mzablocki/anaconda3/lib -shared ccvwrapper.o ccvwrapper_wrap.o -ljpeg -lpython3.7m -lccv `cat /home/mzablocki/ccv/lib/.deps` -o _ccvwrapper.so
