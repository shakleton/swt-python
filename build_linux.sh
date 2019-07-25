#!/bin/bash
for file in "ccvwrapper.i ccvwrapper.c ccvwrapper.h"; do
	cp $file ./ccv/lib
done
cd ./ccv/lib

swig -python -py3 ccvwrapper.i
gcc -fpic -c ccvwrapper.c ccvwrapper_wrap.c ccv_algebra.c ccv_basic.c ccv_io.c ccv_cache.c ccv_classic.c ccv_memory.c ccv_output.c ccv_resample.c ccv_sift.c ccv_swt.c ccv_transform.c ccv_util.c ./3rdparty/siphash/siphash24.c -I/home/mzablocki/anaconda3/include/python3.7m
gcc -L/home/mzablocki/anaconda3/lib -shared ccvwrapper.o ccvwrapper_wrap.o ccv_algebra.o ccv_basic.o ccv_io.o ccv_cache.o ccv_classic.o ccv_memory.o ccv_output.o ccv_resample.o ccv_sift.o ccv_swt.o ccv_transform.o ccv_util.o siphash24.o -ljpeg -lpython3.7m -o _ccvwrapper.so
