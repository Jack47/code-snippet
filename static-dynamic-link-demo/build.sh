#!/bin/sh
g++ -c func_.cpp 
g++ -c main.cpp 
ar rcs libfunc.a func_.o
g++ -shared -fPIC -o libfunc.so func_.cpp
g++ -o main_static main.o -L./ -Wl,-Bstatic -lfunc -Wl,-Bdynamic
g++ -o main_dynamic main.o -L./ -lfunc
