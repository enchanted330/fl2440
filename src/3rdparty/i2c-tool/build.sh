#!/bin/sh

#+--------------------------------------------------------------------------------------------
#|Description:  This shell script used to download i2c-tool source code and cross compile it.
#|     Author:  GuoWenxue <guowenxue@gmail.com>
#|  ChangeLog:
#|           1, Initialize 1.0.0 on 2011.12.23
#+--------------------------------------------------------------------------------------------
. ../scripts/funcs.sh
clear_crossenv

APP_NAME=i2c-tools
DIR_NAME="i2c-tools-3.1.2"
PACK_SUFIX="tar.bz2"

DL_ADDR="https://fossies.org/linux/misc/${DIR_NAME}.${PACK_SUFIX}"
#INST_PATH=`pwd`/../mnt/usr/sbin/

ARCH=arm920t
LINK_STATIC=YES

select_arch
if [ "x86" != "$ARCH" ] ; then
    CROSS="/opt/buildroot-2012.08/${ARCH}/usr/bin/arm-linux-"
    set_crosstool $CROSS
    CONFIG_CROSS=--host=arm-linux
fi
PREFIX_PATH=/apps/${ARCH}/${APP_NAME}


echo "== Download and decompress source code packet =="
download $DL_ADDR
decompress_packet $DIR_NAME $PACK_SUFIX
#svn co http://lm-sensors.org/svn/i2c-tools/trunk $DIR_NAME

if [ "$LINK_STATIC" == "YES" ] ; then
    export CFLAGS=--static
    export LDFLAGS=-static
    CONFIG_LD_STATUS='--enable-static --disable-shared'
fi

echo "+------------------------------------------------------------------+"
echo "|  Platform:  $DIR_NAME for $ARCH  "
echo "|  Compiler:  ${CROSS}gcc  "
echo "+------------------------------------------------------------------+"
mkdir -p $PREFIX_PATH

cd $DIR_NAME
    set -x
    
    FILE=Makefile
    line=`sed -n '/^CC/=' $FILE`
    if [ ! -z $line ] ; then
        sed -i -e ${line}s"|.*|CC=${CROSS}gcc |" $FILE
    fi

    line=`sed -n '/^AR/=' $FILE`
    if [ ! -z $line ] ; then
        sed -i -e ${line}s"|.*|AR=${CROSS}ar |" $FILE
    fi

    FILE=eepromer/Makefile

    line=`sed -n '/^CFLAGS/=' $FILE`
    if [ ! -z $line ] ; then
        sed -i -e ${line}a"LDFLAGS+=-L../lib -li2c -static" $FILE
    fi

    line=`sed -n '/^eeprog:/=' $FILE`
    if [ ! -z $line ] ; then
        sed -i -e ${line}a"\\\t\${CC} \$^ -o \$@ \${LDFLAGS}" $FILE
    fi

    #sed -i -e `expr $line + 1`a"LDFLAGS+=\"-Llib -li2c -static\"" $FILE
    make && make -C eepromer

    cp tools/i2cdetect $PREFIX_PATH
    cp tools/i2cdump $PREFIX_PATH
    cp tools/i2cget $PREFIX_PATH
    cp tools/i2cset $PREFIX_PATH
    cp eepromer/eeprog $PREFIX_PATH
    cp eepromer/eeprom $PREFIX_PATH
    cp eepromer/eepromer $PREFIX_PATH
    ${STRIP} $PREFIX_PATH/*

    if [ -n "$INST_PATH" -a -d "$INST_PATH" ] ; then
        cp $PREFIX_PATH $INST_PATH
    fi
    set +x

    echo "+------------------------------------------------------------------+"
    echo "|   Install Prefix: $PREFIX_PATH"
    echo "|   Install   Path: $INST_PATH"
    echo "+------------------------------------------------------------------+"
cd -

