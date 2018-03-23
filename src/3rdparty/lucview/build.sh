#!/bin/sh

#+--------------------------------------------------------------------------------------------
#|Description:  This shell script used to download lrzsz source code and cross compile it.
#|     Author:  GuoWenxue <guowenxue@gmail.com>
#|      Usage:  Run as "/apps/tools >: luvcview -d /dev/video1  -s 480x272"
#|  ChangeLog:
#|           1, Initialize 1.0.0 on 2011.04.21
#+--------------------------------------------------------------------------------------------

PRJ_PATH=`pwd`

APP_NAME="luvcview-20070512"
PACK_SUFIX="tar.gz"
DL_ADDR="http://nchc.dl.sourceforge.net/project/v4l-lib/Luvcview/$APP_NAME.$PACK_SUFIX"
INST_PATH=${PRJ_PATH}/../mnt/usr/bin/
PREFIX_PATH=${PRJ_PATH}/install


ARCH=arm920t
CROSS=

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

if [ -z $ARCH ] ; then
  select_arch
fi

CROSS="/opt/buildroot-2012.08/${ARCH}/usr/bin/arm-linux-"

if [ ! -d $PREFIX_PATH ] ; then 
    mkdir -p $PREFIX_PATH
fi

if [ -s ${PREFIX_PATH}/bin/luvcview ] ; then
    echo "$APP_NAME already compile and installed, exit now..."
    exit
fi


LIB_SDL=SDL-1.2.15
if [ ! -s $LIB_SDL.zip ] ; then
    wget https://www.libsdl.org/release/SDL-1.2.15.zip
    if [ ! -s $LIB_SDL.zip ] ; then
        echo "ERROR: Download $LIB_SDL failure..."
        exit ;
    fi
    exit ;
fi

if [ ! -d $LIB_SDL ] ; then
    decompress_packet $LIB_SDL.zip
    if [ ! -d $LIB_SDL ] ; then
        echo "ERROR: $LIB_SDL directory not exist..."
        exit ;
    fi
fi


if [ ! -s $PREFIX_PATH/lib/libSDL.a ] ; then 
  cd $LIB_SDL
   export CC=${CROSS}gcc
   export AR=${CROSS}ar
   export LD=${CROSS}ld
   export AS=${CROSS}as
   export RANLIB=${CROSS}ranlib
   CFLAGS+=-static ./configure --host=arm-linux --prefix=$PREFIX_PATH --disable-audio --disable-joystick --disable-cdrom \
   --disable-oss --disable-alsa --disable-esd --disable-pulseaudio --disable-arts --disable-nas --disable-diskaudio \
   --disable-dummyaudio --disable-mintaudio --disable-nasm  --disable-altivec --disable-ipod --disable-video-nanox \
   --disable-video-x11 --disable-dga 
   make && make install
  cd -
fi

# Download source code packet
if [ ! -s $APP_NAME.$PACK_SUFIX ] ; then
   echo "+------------------------------------------------------------------+"
   echo "|  Download $APP_NAME.$PACK_SUFIX now "  
   echo "+------------------------------------------------------------------+"

   wget $DL_ADDR
fi

# Decompress source code packet
if [ ! -d $APP_NAME ] ; then
    decompress_packet $APP_NAME.$PACK_SUFIX
fi


echo "+------------------------------------------------------------------+"
echo "|          Build $APP_NAME for $ARCH "
echo "| Crosstool:  $CROSS"
echo "+------------------------------------------------------------------+"

cd $APP_NAME
   export CC=${CROSS}gcc
   export AR=${CROSS}ar
   export LD=${CROSS}ld
   export AS=${CROSS}as
   export RANLIB=${CROSS}ranlib
   patch -p1 < ../patch/luvcview-20070512-arm.patch
   make 
   cp luvcview ${PREFIX_PATH}/bin
cd -

