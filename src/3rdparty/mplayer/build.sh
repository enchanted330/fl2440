#!/bin/sh

#+--------------------------------------------------------------------------------------------
#|Description:  This shell script used to download file source code and cross compile it.
#|     Author:  GuoWenxue <guowenxue@gmail.com>
#|  ChangeLog:
#|           1, Initialize 1.0.0 on 2012.03.28
#+--------------------------------------------------------------------------------------------

PRJ_PATH=`pwd`

if [ -z "$INST_PATH" ] ; then 
    INST_PATH=${PRJ_PATH}/install
fi 

if [ ! -d $INST_PATH ] ; then
    mkdir -p $INST_PATH
fi

ARCH=arm920t

if [ -z "$ARCH" -a $# -gt 0 ] ; then
   ARCH=$1
fi

sup_arch=("" "arm926t" "arm920t" )

function select_arch()
{
   echo "Current support ARCH: "
   i=1
   len=${#sup_arch[*]}

   while [ $i -lt $len ]; do
     echo "$i: ${sup_arch[$i]}"
     let i++;
   done

   echo "Please select: "
   index=
   read index
   ARCH=${sup_arch[$index]}
}


function decompress_packet()
(
   echo "+---------------------------------------------+"
   echo "|  Decompress $1 now"  
   echo "+---------------------------------------------+"

    ftype=`file "$1"`
    case "$ftype" in
       "$1: Zip archive"*)
           unzip "$1" ;;
       "$1: gzip compressed"*)
           if [ 0 != `expr "$1" : ".*.tar.*" ` ] ; then
               tar -xzf $1
           else
               gzip -d "$1"
           fi ;;
       "$1: bzip2 compressed"*)
           if [ 0 != `expr "$1" : ".*.tar.*" ` ] ; then
               tar -xjf $1
           else
               bunzip2 "$1"
           fi ;;
       "$1: POSIX tar archive"*)
           tar -xf "$1" ;;
       *)
          echo "$1 is unknow compress format";;
    esac
)


if [ -z "$CROSS" ] ; then
   if [ -z $ARCH ] ; then
      select_arch
   fi
   CROSS="/opt/buildroot-2012.08/${ARCH}/usr/bin/arm-linux-"
fi

CROSS_PATH=`dirname $CROSS`
export PATH=${CROSS_PATH}:$PATH
export CC=${CROSS}gcc
export CXX=${CROSS}g++
export AR=${CROSS}ar
export AS=${CROSS}as
export LD=${CROSS}ld
export NM=${CROSS}nm
export RANLIB=${CROSS}ranlib
export STRIP=${CROSS}strip

LIBMAD_NAME=libmad-0.15.1b
LIBMAD_DL_ADDR=ftp://ftp.mars.org/pub/mpeg/$LIBMAD_NAME.tar.gz
if [ ! -d ${INST_PATH}/libmad/include ]; then

    echo "+------------------------------------------------------------------+"
    echo "|          Build libmad(MPEG Audio Decoder) for $ARCH "
    echo "| Crosstool:  $CROSS"
    echo "+------------------------------------------------------------------+"

    if [ ! -s $LIBMAD_NAME.tar.gz ] ; then
        wget $LIBMAD_DL_ADDR
    fi

    decompress_packet $LIBMAD_NAME.tar.gz
    cd $LIBMAD_NAME
    ./configure --enable-fpm=arm --host=arm-linux --disable-shared \
    --disable-debugging --prefix=$INST_PATH/libmad CC=${CC}

    #Modify the Makefile to fix the cross compiler bug
    sed -i -e 's/-fforce-mem//' Makefile

    make && make install

    cd -
fi

MADID3_NAME=libid3tag-0.15.1b
MADID3_DL_ADDR=ftp://ftp.mars.org/pub/mpeg/$MADID3_NAME.tar.gz
if [ ! -d ${INST_PATH}/madid3 ]; then

    echo "+------------------------------------------------------------------+"
    echo "|          Build madid3(MPEG Audio Decoder) for $ARCH "
    echo "| Crosstool:  $CROSS"
    echo "+------------------------------------------------------------------+"

    if [ ! -s $MADID3_NAME.tar.gz ] ; then 
        wget $MADID3_DL_ADDR
    fi

    decompress_packet $MADID3_NAME.tar.gz
    cd $MADID3_NAME
    ./configure --host=arm-linux --prefix=$INST_PATH/madid3 --disable-shared --disable-debugging \
    CFLAGS=-I${INST_PATH}/libmad/include LDFLAGS=-L${INST_PATH}/libmad/lib CC=${CC}

    #Modify the Makefile to fix the cross compiler bug
    sed -i -e 's/-fforce-mem//' Makefile

    make && make install

    cd -
fi

MADPLAY_NAME=madplay-0.15.2b
MADPLAY_DL_ADDR=ftp://ftp.mars.org/pub/mpeg/$MADPLAY_NAME.tar.gz
if [ ! -d ${INST_PATH}/madplay ]; then

    echo "+------------------------------------------------------------------+"
    echo "|          Build madplay(MPEG Audio Decoder Player) for $ARCH "
    echo "| Crosstool:  $CROSS"
    echo "+------------------------------------------------------------------+"

    if [ ! -s $MADPLAY_NAME.tar.gz ] ; then
        wget $MADPLAY_DL_ADDR
    fi

    decompress_packet $MADPLAY_NAME.tar.gz
    cd $MADPLAY_NAME
    ./configure --host=arm-linux --prefix=$INST_PATH/madplay --disable-shared --disable-debugging \
    CFLAGS="-I${INST_PATH}/libmad/include -I${INST_PATH}/madid3/include " \
    LDFLAGS="-L${INST_PATH}/libmad/lib -L${INST_PATH}/madid3/lib" CC=${CC}

    #Modify the Makefile to fix the cross compiler bug
    sed -i -e 's/-fforce-mem//' Makefile

    make && make install

    cd -
fi

APP_NAME="MPlayer-1.0rc3"
PACK_SUFIX="tar.bz2"
DL_ADDR="http://www.mplayerhq.hu/MPlayer/releases/MPlayer-1.0rc3.tar.bz2"

echo "+------------------------------------------------------------------+"
echo "|          Build $APP_NAME for $ARCH "
echo "| Crosstool:  $CROSS"
echo "+------------------------------------------------------------------+"

# Download source code packet
if [ ! -s $APP_NAME.$PACK_SUFIX ] ; then
   echo "+------------------------------------------------------------------+"
   echo "|  Download $APP_NAME.$PACK_SUFIX  now "  
   echo "+------------------------------------------------------------------+"

   wget $DL_ADDR
fi

# Decompress source code packet
if [ -d $APP_NAME ] ; then
    rm -rf $APP_NAME
fi
decompress_packet $APP_NAME.$PACK_SUFIX


cd $APP_NAME

#Modify the source code to fix the build bug
FILE=libmpeg2/motion_comp_arm_s.S
line=`sed -n '/.text/=' $FILE`
line=`expr $line - 1`
sed -i -e `expr $line`a"#ifndef HAVE_PLD" $FILE
sed -i -e `expr $line + 1`a"    .macro pld reg" $FILE
sed -i -e `expr $line + 2`a"    .endm" $FILE
sed -i -e `expr $line + 3`a"#endif" $FILE

./configure --prefix=$INST_PATH/mplayer --cc=arm-linux-gcc  --ar=arm-linux-ar  \
 --ranlib=arm-linux-ranlib --disable-gui   \
 --target=arm-armv4-linux --host-cc=gcc  --disable-freetype\
 --enable-fbdev --disable-mencoder --disable-sdl --disable-live  --disable-dvdread \
 --disable-libdvdcss-internal --disable-x11 --enable-cross-compile  --disable-mp3lib \
 --enable-ass  \
 --disable-dvdnav  --disable-dvdread-internal    --disable-jpeg --disable-tga \
 --disable-dvbhead --disable-pnm --disable-tv --disable-ivtv \
 --disable-fontconfig --disable-xanim --disable-win32dll --disable-armv5te --disable-armv6

    make && make install
    ${CROSS}strip $INST_PATH/mplayer/bin/mplayer
cd -

