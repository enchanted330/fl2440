#!/bin/sh

#+--------------------------------------------------------------------------------------------
#|Description:  This shell script used to download tree wireless_tools code and cross compile it.
#|     Author:  GuoWenxue <guowenxue@gmail.com>
#|  ChangeLog:
#|           1, Initialize 1.0.0 on 2011.12.23
#+--------------------------------------------------------------------------------------------

PRJ_PATH=`pwd`

APP_NAME="openssl-1.0.1c"
PACK_SUFIX="tar.gz"
DL_ADDR="http://www.openssl.org/source/$APP_NAME.$PACK_SUFIX"
INST_PATH=$PRJ_PATH/install
#INST_PATH=../install

ARCH=arm926t
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

if [ ! -d $INST_PATH ] ; then
    mkdir -p $INST_PATH
else
    echo "$APP_NAME already cross compiled, exit now..."
    exit;
fi

echo "+------------------------------------------------------------------+"
echo "|          Build $APP_NAME for $ARCH "
echo "| Crosstool:  $CROSS"
echo "+------------------------------------------------------------------+"

cd $APP_NAME
   set -x
   patch -p1 < ../patch/$APP_NAME-tls-extensions.patch 

   #Modify the compiler
   FILE=Makefile
   line=`sed -n '/^CC= cc/=' $FILE` 
   if [ -n "$line" ] ; then 
       sed -i -e ${line}s"|.*|CC = ${CROSS}gcc|" $FILE 
   fi

   line=`sed -n '/^AR=ar/=' $FILE` 
   if [ -n "$line" ] ; then 
       sed -i -e ${line}s"|.*|AR = ${CROSS}ar \$(ARFLAGS) r|" $FILE 
   fi
   
   line=`sed -n '/^RANLIB= /=' $FILE` 
   if [ -n "$line" ] ; then 
       sed -i -e ${line}s"|.*|RANLIB = ${CROSS}ranlib|" $FILE 
   fi

   #Support build shared library
   line=`sed -n '/^OPTIONS= /=' $FILE`
   if [ -n "$line" ] ; then
       sed -i -e ${line}s"|.*|OPTIONS=no-ec_nistp_64_gcc_128 no-gmp no-jpake no-krb5 no-md2 no-rc5 no-rfc3779 no-sctp enalbe-shared no-store no-zlib no-zlib-dynamic static-engine|" $FILE
   fi


   line=`sed -n '/^SHLIB_EXT=/=' $FILE`
   if [ -n "$line" ] ; then
       sed -i -e ${line}s"|.*|SHLIB_EXT=.so.\$(SHLIB_MAJOR).\$(SHLIB_MINOR)|" $FILE
   fi

   line=`sed -n '/^SHLIB_TARGET/=' $FILE`
   if [ -n "$line" ] ; then
       sed -i -e ${line}s"|.*|SHLIB_TARGET=linux-shared|" $FILE
   fi

   line=`sed -n '/^build_all:/=' $FILE`
   if [ -n "$line" ] ; then
       sed -i -e ${line}s"|.*|build_all: build_libs build_apps build_tests build_tools build-shared|" $FILE
   fi

   #Modify install path
   line=`sed -n '/^INSTALLTOP=/=' $FILE`
   if [ -n "$line" ] ; then
       sed -i -e ${line}s"|.*|INSTALLTOP = $INST_PATH|" $FILE
   fi

   line=`sed -n '/^OPENSSLDIR=/=' $FILE`
   if [ -n "$line" ] ; then
       sed -i -e ${line}s"|.*|OPENSSLDIR = $INST_PATH|" $FILE
   fi

   make && make install
   $STRIP *.so*
   cp -af *.so* $INST_PATH/lib
cd -

