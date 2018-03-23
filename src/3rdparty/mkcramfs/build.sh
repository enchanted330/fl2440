#!/bin/sh

#+--------------------------------------------------------------------------------------------
#|Description:  This shell script used to download mkcramfs source code and compile it for X86
#|              and cross compile it for ARM Linux, all is static cross compile.
#|     Author:  GuoWenxue <guowenxue@gmail.com>
#|  ChangeLog:
#|           1, Initialize 1.0.0 on 2011.04.12
#+--------------------------------------------------------------------------------------------
. ../scripts/funcs.sh
clear_crossenv

PRJ_PATH=`pwd`

ARCH=x86
LINK_STATIC=YES

# Download and decompress source code packet 
CRAMFS="cramfs-1.1"
PACK_SUFIX=tar.gz
DL_ADDR=http://nchc.dl.sourceforge.net/project/cramfs/cramfs/1.1/$CRAMFS.$PACK_SUFIX

download $DL_ADDR

if [ "$LINK_STATIC" == "YES" ] ; then
    export CFLAGS=--static
    export LDFLAGS=-static
fi

echo "+------------------------------------------------------------------+"
echo "|  Platform:  $CRAMFS for $ARCH  "
echo "|  Compiler:  ${CROSS}gcc  "
echo "+------------------------------------------------------------------+"

if [ ! -s $CRAMFS/mkcramfs ] ; then
    tar -xzf $CRAMFS.$PACK_SUFIX
    cd $CRAMFS
    make
    cd -
fi

