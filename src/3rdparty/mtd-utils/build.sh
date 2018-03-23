#!/bin/sh

#+--------------------------------------------------------------------------------------------
#|Description:  This shell script used to download lzo,zlib,mtd-utils source code
#|              and cross compile it for ARM Linux, all is static cross compile.
#|     Author:  GuoWenxue <guowenxue@gmail.com>
#|  ChangeLog:
#|           1, Initialize 1.0.0 on 2011.04.12
#+--------------------------------------------------------------------------------------------
. ../scripts/funcs.sh
clear_crossenv

PRJ_PATH=`pwd`


ARCH=x86
#ARCH=arm920t
LINK_STATIC=YES

select_arch
if [ "x86" != "$ARCH" ] ; then
    #CROSS="/opt/buildroot-2011.11/${ARCH}/usr/bin/arm-linux-"
    CROSS="/opt/buildroot-2012.08/${ARCH}/usr/bin/arm-linux-"
    set_crosstool $CROSS
    CONFIG_CROSS=--host=arm-linux
fi

# Download and decompress source code packet 
LZO="lzo-2.04"
PACK_SUFIX=tar.gz
DL_ADDR=http://www.oberhumer.com/opensource/lzo/download/$LZO.$PACK_SUFIX
download $DL_ADDR


if [ "$LINK_STATIC" == "YES" ] ; then
    export CFLAGS=--static
    export LDFLAGS=-static
    CONFIG_LD_STATUS='--enable-static'
fi
echo "+------------------------------------------------------------------+"
echo "|  Platform:  $LZO for $ARCH  "
echo "|  Compiler:  ${CROSS}gcc  "
echo "+------------------------------------------------------------------+"

# Cross compile lzo
if [ ! -s $LZO/src/.libs/liblzo*.a ] ; then
    decompress_packet $LZO $PACK_SUFIX

    cd  $LZO
    ./configure ${CONFIG_CROSS} ${CONFIG_LD_STATUS}
    make
    cd -
fi

ZLIB="zlib-1.2.5"
PACK_SUFIX=tar.gz
#DL_ADDR=http://down1.chinaunix.net/distfiles/$ZLIB.$PACK_SUFIX
DL_ADDR=http://nchc.dl.sourceforge.net/project/freefoam/ThirdParty/zlib/$ZLIB.$PACK_SUFIX
download $DL_ADDR

if [ "$LINK_STATIC" == "YES" ] ; then
    export CFLAGS=--static
    export LDFLAGS=-static
    CONFIG_LD_STATUS='--static'
fi
echo "+------------------------------------------------------------------+"
echo "|  Platform:  $ZLIB for $ARCH  "
echo "|  Compiler:  ${CROSS}gcc  "
echo "+------------------------------------------------------------------+"

#Cross compile zlib
if [ ! -s $ZLIB/libz.a ] ; then
    decompress_packet $ZLIB $PACK_SUFIX
    cd  $ZLIB
    ./configure ${CONFIG_LD_STATUS}
    make
    cd  -
fi


e2fsprogs_ver=1.42
PACK_SUFIX=tar.gz
DL_ADDR=http://nchc.dl.sourceforge.net/project/e2fsprogs/e2fsprogs/$e2fsprogs_ver/e2fsprogs-$e2fsprogs_ver.$PACK_SUFIX
download $DL_ADDR

unset CFLAGS
unset LDFLAGS

echo "+------------------------------------------------------------------+"
echo "|  Platform:  e2fsprogs-$e2fsprogs_ver for $ARCH  "
echo "|  Compiler:  ${CROSS}gcc  "
echo "+------------------------------------------------------------------+"

if [ ! -s e2fsprogs-$e2fsprogs_ver/lib/libuuid.a ] ; then
    decompress_packet e2fsprogs-$e2fsprogs_ver $PACK_SUFIX
    cd e2fsprogs-$e2fsprogs_ver
    ./configure ${CONFIG_CROSS} --build=i686-pc-linux-gnu --enable-elf-shlibs
    make
    cd -
fi

mtd="mtd-utils-1.4.9"
e2fsprogs_ver=1.42
PACK_SUFIX=tar.bz2
DL_ADDR=ftp://ftp.infradead.org/pub/mtd-utils/${mtd}.$PACK_SUFIX
download $DL_ADDR
# download mtd-utils source code
#if [ ! -d  mtd-utils* ] ; then
   #git clone git://git.infradead.org/mtd-utils.git
#fi

echo "+------------------------------------------------------------------+"
echo "|  Platform:  $mtd for $ARCH  "
echo "|  Compiler:  ${CROSS}gcc  "
echo "+------------------------------------------------------------------+"

unset CFLAGS
unset LDFLAGS

decompress_packet $mtd $PACK_SUFIX
cd $mtd

head -1 common.mk | grep "CROSS="
if [ 0 != $? ] ; then 
    echo "Modify file common.mk" 
    sed -i -e 1i"CROSS=$CROSS" common.mk 
fi 

line=`sed -n '/CFLAGS ?= -O2 -g/=' common.mk ` 
if [ ! -z $line ] ; then 
    sed -i -e ${line}s"|.*|CFLAGS ?= -O2 -g --static|" common.mk 
fi

export CFLAGS="-DWITHOUT_XATTR -I$PRJ_PATH/$ZLIB -I$PRJ_PATH/$LZO/include -I$PRJ_PATH/e2fsprogs-$e2fsprogs_ver/lib"
export ZLIBLDFLAGS=-L$PRJ_PATH/$ZLIB
export LZOLDFLAGS=-L$PRJ_PATH/$LZO/src/.libs/
export LDFLAGS="-static -L $PRJ_PATH/e2fsprogs-$e2fsprogs_ver/lib $ZLIBLDFLAGS $LZOLDFLAGS"
make

exit

set -x
cd arm-linux/
${CROSS}strip nandwrite flash_erase  nanddump 
sudo cp nandwrite $INST_PATH/.nandwrite
sudo cp flash_erase $INST_PATH/.flash_erase
sudo cp nanddump $INST_PATH/.nanddump

