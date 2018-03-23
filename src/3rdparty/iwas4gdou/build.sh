#!/bin/sh

#+--------------------------------------------------------------------------------------------
#|Description:  This shell script used to download SQLite code and cross compile it.
#|     Author:  GuoWenxue <guowenxue@gmail.com>
#|  ChangeLog:
#|           1, Initialize 1.0.0 on 2011.12.26
#+--------------------------------------------------------------------------------------------

PRJ_PATH=`pwd`

APP_NAME="libpcap-1.1.1"
PACK_SUFIX="tar.gz"
PREFIX_PATH=$PRJ_PATH/install

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

if [ "arm920t" != $ARCH  -a "arm926t" != $ARCH ] ; then 
    echo "+------------------------------------------------------------------+"
    echo "|  ERROR: Unsupport platform $ARCH to cross compile "  
    echo "+------------------------------------------------------------------+"
    exit -1;
else
    CROSS="/opt/buildroot-2012.08/${ARCH}/usr/bin/arm-linux-"
fi 

export CC=${CROSS}gcc 
export CXX=${CROSS}g++ 
export AR=${CROSS}ar 
export AS=${CROSS}as 
export LD=${CROSS}ld 
export NM=${CROSS}nm 
export RANLIB=${CROSS}ranlib 
export STRIP=${CROSS}strip
export LDFLAGS=-static
export CFLAGS=--static

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

if [ ! -d $PREFIX_PATH ] ; then
    mkdir -p $PREFIX_PATH
else
    echo "$APP_NAME already cross compiled, exit now..."
    exit;
fi

echo "+------------------------------------------------------------------+"
echo "|          Build $APP_NAME for $ARCH "
echo "| Crosstool:  $CROSS"
echo "+------------------------------------------------------------------+"

cd $APP_NAME
   patch -p1 < $PRJ_PATH/${APP_NAME}-arm.patch
   set -x
   export LDFLAGS+=-lc
   ./configure --host=arm-linux --prefix=$PREFIX_PATH --disable-shared --enable-static \
   --disable-ipv6 --disable-largefile --disable-can --disable-bluetooth
   make
   make install
cd -

APP_NAME="iwas4gdou-0.1"
PACK_SUFIX="tar.bz2"

# Decompress source code packet 
if [ ! -d $APP_NAME ] ; then 
    decompress_packet $APP_NAME.$PACK_SUFIX 
fi

cd $APP_NAME
   sed -i -e "s|^CC.*|CC          = ${CROSS}gcc|g" Makefile
   sed -i -e "s|^INCLUDE.*|INCLUDE          = -I${PREFIX_PATH}/include|g" Makefile
   sed -i -e "s|^LIB.*|LIB          = -L${PREFIX_PATH}/lib|g" Makefile
   make
   ${CROSS}strip iwas4gdou
   cp iwas4gdou ${PREFIX_PATH}/bin
cd -
