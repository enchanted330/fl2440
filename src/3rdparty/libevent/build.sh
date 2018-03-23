#!/bin/sh

#+--------------------------------------------------------------------------------------------
#|Description:  This shell script used to download libevent library source code and cross compile it.
#|     Author:  GuoWenxue <guowenxue@gmail.com>
#|  ChangeLog:
#|           1, Initialize 1.0.0 on 2014.01.24
#+--------------------------------------------------------------------------------------------
. ../scripts/funcs.sh
clear_crossenv

ARCH=arm920t
#ARCH=x86
select_arch
if [ "x86" != "$ARCH" ] ; then
    CROSS="/opt/buildroot-2011.11/${ARCH}/usr/bin/arm-linux-"
    set_crosstool $CROSS
fi
set_env

APP_NAME=libevent
DIR_NAME="libevent-2.0.21-stable"
PACK_SUFIX="tar.gz"
DL_ADDR="https://github.com/downloads/libevent/libevent/$DIR_NAME.$PACK_SUFIX"

OPENSSL_INC=${APPS_PATH}/openssl/include/
OPENSSL_LIB=${APPS_PATH}/openssl/lib
export CFLAGS+="-I${OPENSSL_INC} "
export LDFLAGS+="-L${OPENSSL_LIB} "

PREFIX_PATH=${APPS_PATH}/${APP_NAME}
#LINK_STATIC=YES

if [ ! -d ${OPENSSL_INC} ] ; then
    cd ../openssl/
    sh build.sh
    cd -
fi

mkdir -p $PREFIX_PATH
if [ "$LINK_STATIC" == "YES" ] ; then
    export CFLAGS+="--static "
    export LDFLAGS+="-static "
    CONFIG_LD_STATUS='--enable-static --disable-shared'
fi


# Download and decompress source code packet 
download $DL_ADDR
decompress_packet $DIR_NAME $PACK_SUFIX


echo "+------------------------------------------------------------------+"
echo "|  Platform:  $DIR_NAME for $ARCH  "
echo "|  Compiler:  ${CROSS}gcc  "
echo "+------------------------------------------------------------------+"

cd $DIR_NAME
   set -x
   ./configure --prefix=$PREFIX_PATH ${CONFIG_CROSS} ${CONFIG_LD_STATUS} \
   --enable-thread-support --enable-openssl --enable-function-sections 
   make && make install

   if [ -n "$INST_PATH" -a -d "$INST_PATH" ] ; then
       cp $PREFIX_PATH/lib/*.so* $INST_PATH
   fi
   set +x

   echo "+------------------------------------------------------------------+"
   echo "|   Install Prefix: $PREFIX_PATH"
   echo "|   Install   Path: $INST_PATH"
   echo "+------------------------------------------------------------------+"
cd -

