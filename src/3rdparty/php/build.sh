#!/bin/sh

#+--------------------------------------------------------------------------------------------
#|Description:  This shell script used to download libconfig library source code and cross compile it.
#|     Author:  GuoWenxue <guowenxue@gmail.com>
#|  ChangeLog:
#|           1, Initialize 1.0.0 on 2011.12.23
#+--------------------------------------------------------------------------------------------
. ../scripts/funcs.sh
clear_crossenv

ARCH=arm920t
select_arch
if [ "x86" != "$ARCH" ] ; then
    CROSS="/opt/buildroot-2012.08/${ARCH}/usr/bin/arm-linux-"
    set_crosstool $CROSS
    set_env
fi

APP_PATH=/apps/${ARCH}/
APP_NAME=php
DIR_NAME=php-5.4.10
PACK_SUFIX="tar.bz2"
#Unsupported History Releases Version: http://www.php.net/releases
DL_ADDR="http://museum.php.net/php5/$DIR_NAME.$PACK_SUFIX"


LINK_STATIC=YES
PREFIX_PATH=${APP_PATH}/${APP_NAME}
mkdir -p $PREFIX_PATH
if [ "$LINK_STATIC" == "YES" ] ; then
    CONFIG_LD_STATUS='--enable-static'
fi

# Download and decompress source code packet 
download $DL_ADDR
decompress_packet $DIR_NAME $PACK_SUFIX

echo "+------------------------------------------------------------------+"
echo "|  Platform:  $DIR_NAME for $ARCH  "
echo "|  Compiler:  ${CROSS}gcc  "
echo "+------------------------------------------------------------------+"
mkdir -p $PREFIX_PATH

cd $DIR_NAME
   set -x

   sed -i -e "s/uint _res/uint/g"  main/php_output.h

   line=`sed -n '/Your system seems to lack POSIX threads/=' configure`
   sed -i -e ${line}s'|.*|{ echo "configure: error: Your system seems to lack POSIX threads." 1>\&2;}|' configure
   ./configure  --prefix=$PREFIX_PATH ${CONFIG_CROSS} --disable-rpath \
   --disable-cli --enable-bcmath --enable-calendar --enable-maintainer-zts \
   --enable-embed=shared --enable-force-cgi-redirect --disable-ftp \
   --enable-inline-optimization  --disable-magic-quotes --enable-memory-limit \
   --disable-safe-mode --enable-sockets --enable-track-vars --enable-trans-sid \
   --enable-wddx --sysconfdir=/apps/appWeb/etc --with-pic \
   --with-db --with-regex=system --with-pear --without-zlib --without-iconv \
   --disable-dom --disable-libxml --disable-simplexml --disable-xml --disable-wddx \
   --disable-xmlreader --without-xmlrpc --disable-xmlwriter --with-config-file-path=/apps/appweb
   LDFLAGS+="-lpthread -ldl" \
   make && make install

   $STRIP $PREFIX_PATH/lib/*.so*

   if [ -n "$INST_PATH" -a -d "$INST_PATH" ] ; then
       cp $PREFIX_PATH/lib/*.so* $INST_PATH
   fi
   set +x

   echo "+------------------------------------------------------------------+"
   echo "|   Install Prefix: $PREFIX_PATH"
   echo "|   Install   Path: $INST_PATH"
   echo "+------------------------------------------------------------------+"
cd 

