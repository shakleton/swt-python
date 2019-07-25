#!/bin/bash

swig -python ccvwrapper.i
gcc -fpic -c ccvwrapper.c ccvwrapper_wrap.c ccv_algebra.c ccv_basic.c ccv_cache.c ccv_classic.c ccv_io.c ccv_memory.c ccv_output.c ccv_resample.c ccv_sift.c ccv_swt.c ccv_transform.c ccv_util.c ./3rdparty/siphash/siphash24.c -I/Library/Frameworks/Python.framework/Versions/3.7/include/python3.7m
gcc -arch x86_64 -L/Library/Frameworks/Python.framework/Versions/3.7/lib -flat_namespace -undefined suppress -shared ccvwrapper.o ccvwrapper_wrap.o ccv_algebra.o ccv_basic.o ccv_cache.o ccv_classic.o ccv_io.o ccv_memory.o ccv_output.o ccv_resample.o ccv_sift.o ccv_swt.o ccv_transform.o ccv_util.o siphash24.o -I/anaconda3/lib -ljpeg -lpython3.7 -o _ccvwrapper.so
