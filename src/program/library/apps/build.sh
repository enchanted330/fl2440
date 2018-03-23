#!/bin/sh

#LINK_FLAG=static

if [ "$LINK_FLAG" = "static" ] ; then 
    gcc main.c -I../vendor1 -I../vendor2 -L../vendor1 -lmodule1 -L../vendor2 -lmodule2 -static
else
    gcc main.c -I../vendor1 -I../vendor2 -L../vendor1 -lmodule1 -L../vendor2 -lmodule2 
fi
