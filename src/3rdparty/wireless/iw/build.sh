#!/bin/sh

#+--------------------------------------------------------------------------------------------
#|Description:  This shell script used to download file source code and cross compile it.
#|     Author:  GuoWenxue <guowenxue@gmail.com>
#|  ChangeLog:
#|           1, Initialize 1.0.0 on 2011.04.21
#+--------------------------------------------------------------------------------------------
. ../../scripts/funcs.sh
clear_crossenv

ARCH=arm920t
LINK_STATIC=YES
PRJ_PATH=`pwd`
INST_PATH=${PRJ_PATH}/bin

select_arch
if [ "x86" != "$ARCH" ] ; then
    CROSS="/opt/buildroot-2011.11/${ARCH}/usr/bin/arm-linux-"
    set_crosstool $CROSS
    CONFIG_CROSS=--host=arm-linux
fi 

if [ "$LINK_STATIC" == "YES" ] ; then
    export CFLAGS=--static
    export LDFLAGS+="-static"
    CONFIG_LD_STATUS='--disable-shared --enable-static'
else
    CONFIG_LD_STATUS='--enable-shared --disable-static'
fi

#Downlink and cross compile libnl
APP_NAME=libnl3
DIR_NAME="libnl-3.2.22"
PACK_SUFIX="tar.gz"
DL_ADDR="http://www.infradead.org/~tgr/libnl/files/$DIR_NAME.$PACK_SUFIX"
#PREFIX_PATH=/apps/${ARCH}/${APP_NAME}
PREFIX_PATH=${PRJ_PATH}/libnl3

if [ ! -d $PREFIX_PATH ] ; then
  # Download and decompress source code packet 
  download $DL_ADDR
  decompress_packet $DIR_NAME $PACK_SUFIX

  echo "+------------------------------------------------------------------+"
  echo "|  Platform:  $DIR_NAME for $ARCH  "
  echo "|  Compiler:  ${CROSS}gcc  "
  echo "+------------------------------------------------------------------+"
  mkdir -p $PREFIX_PATH

  cd $DIR_NAME
    ./configure --prefix=$PREFIX_PATH ${CONFIG_CROSS} ${CONFIG_LD_STATUS} 

    sed -i -e  '/AF_RDS/d' lib/addr.c 
    make && make install

    echo "+------------------------------------------------------------------+"
    echo "|   Install Prefix: $PREFIX_PATH"
    echo "|   Install   Path: $INST_PATH"
    echo "+------------------------------------------------------------------+"
 cd -
fi

#Downlink and cross compile libnl
DIR_NAME="iw-3.10"
PACK_SUFIX="tar.gz"
DL_ADDR="https://www.kernel.org/pub/software/network/iw/$DIR_NAME.$PACK_SUFIX"

# Download and decompress source code packet 
download $DL_ADDR
decompress_packet $DIR_NAME $PACK_SUFIX

echo "+------------------------------------------------------------------+"
echo "|  Platform:  $DIR_NAME for $ARCH  "
echo "|  Compiler:  ${CROSS}gcc  "
echo "+------------------------------------------------------------------+"
mkdir -p $INST_PATH

cd $DIR_NAME
export PKG_CONFIG_PATH=${PREFIX_PATH}/lib/pkgconfig:$PKG_CONFIG_PATH

FILE=Makefile

line=`sed -n '/^CC/=' $FILE`
sed -i -e ${line}s"|.*|CC=${CC}|" $FILE
sed -i -e `expr $line + 1`a'LDFLAGS+=-lpthread -lm' $FILE

line=`sed -n '/-o iw/=' $FILE`
sed -i -e ${line}s"|.*|\t@\$(Q)\$(CC) \$(OBJS) \$(LIBS) \$(LDFLAGS) -o iw|" $FILE

make && $STRIP iw && cp iw ${INST_PATH}
