#!/bin/sh

STA_LIB_NAME=libmodule1.a
DYN_LIB_NAME=libmodule1.so

cd src

#Generate Static library
gcc -c --static *.c
ar -rsv $STA_LIB_NAME  *.o
rm -rf *.o
mv $STA_LIB_NAME ..


#Generate Dynamic library
gcc -fpic -shared *.c -o $DYN_LIB_NAME
rm -f *.o
mv $DYN_LIB_NAME ..

