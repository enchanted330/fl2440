#!/bin/sh

#LINK_FLAG=static
export CFLAGS="-I../vendor1 -I../vendor2"
export LDFLAGS="-L../vendor1 -lmodule1 -L../vendor2 -lmodule2"

if [ "$LINK_FLAG" = "static" ] ; then 
    gcc $CFLAGS  main.c $LDFLAGS
else
    gcc $CFLAGS main.c $LDFLAGS -static
fi
