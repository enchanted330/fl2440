#!/bin/sh

#+--------------------------------------------------------------------------------------------
#|Description:  This shell script used to download file source code and cross compile it.
#|     Author:  GuoWenxue <guowenxue@gmail.com>
#|  ChangeLog:
#|           1, Initialize 1.0.0 on 2012.03.28
#+--------------------------------------------------------------------------------------------

PRJ_PATH=`pwd`
APP_NAME=usb-modeswitch-2.2.1
PACK_SUFIX=tar.bz2
PREFIX_PATH=${PRJ_PATH}/install

ARCH=arm926t

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
   CROSS="/opt/buildroot-2011.11/${ARCH}/usr/bin/arm-linux-"
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
export OBJDUM=${CROSS}objdump
export STRIP=${CROSS}strip

if [ ! -d $PREFIX_PATH ] ; then
    mkdir -p $PREFIX_PATH/bin
else
    echo "$APP_NAME already compile and installed, exit now..."
    exit
fi

            #####################################
            #   Cross compile libusb library    # 
            #####################################

set -x

LIBUSB_NAME=libusb-1.0.18
DL_ADDR="http://ncu.dl.sourceforge.net/project/libusb/libusb-1.0/$LIBUSB_NAME/$LIBUSB_NAME.$PACK_SUFIX"
# Download source code packet
if [ ! -s $LIBUSB_NAME.$PACK_SUFIX ] ; then
   echo "+------------------------------------------------------------------+"
   echo "|  Download $LIBUSB_NAME.$PACK_SUFIX now "  
   echo "+------------------------------------------------------------------+"

   wget $DL_ADDR
fi

# Decompress source code packet
if [ ! -d $LIBUSB_NAME ] ; then
    decompress_packet $LIBUSB_NAME.$PACK_SUFIX
fi


echo "+------------------------------------------------------------------+"
echo "|          Build $LIBUSB_NAME for $ARCH "
echo "| Crosstool:  $CROSS"
echo "+------------------------------------------------------------------+"

cd $LIBUSB_NAME
./configure --prefix=$PREFIX_PATH  --build=i686 --host=arm-linux --disable-shared --enable-static \
--disable-udev  --enable-debug-log --enable-system-log 

exit
make && make install
cd -


            #####################################
            #   Cross compile usb-modemswitch   # 
            #####################################


DL_ADDR="http://www.draisberghof.de/usb_modeswitch/$APP_NAME.$PACK_SUFIX"
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
FILE=Makefile

sed -i -e "s|^CC.*|CC          = ${CROSS}gcc|g" $FILE
sed -i -e "s|^CFLAGS.*|CFLAGS      += -Wall -I../install/include -I../install/include/libusb-1.0 -static|g" $FILE
sed -i -e "s|^LIBS.*|LIBS        = -L../install/lib/ -l usb-1.0 -l pthread|g" $FILE
make
set -x
${STRIP} usb_modeswitch
cp usb_modeswitch $PREFIX_PATH/bin

cd -
