#!/bin/sh

#+--------------------------------------------------------------------------------------------
#|Description:  This shell script used to download tree file code and cross compile it.
#|     Author:  GuoWenxue <guowenxue@gmail.com>
#|  ChangeLog:
#|           1, Initialize 1.0.0 on 2011.04.21
#+--------------------------------------------------------------------------------------------

PRJ_PATH=`pwd`

APP_NAME="tree-1.5.3"
PACK_SUFIX="tar.gz"
#DL_ADDR="http://tree.sourcearchive.com/downloads/1.5.3-2/$APP_NAME.$PACK_SUFIX"
#DL_ADDR="http://www-uxsup.csx.cam.ac.uk/pub/misc/sunfreeware/SOURCES/$APP_NAME.$PACK_SUFIX"
DL_ADDR=http://ftp.riken.jp/Sun/sunfreeware/SOURCES/$APP_NAME.$PACK_SUFIX

INST_PATH=${PRJ_PATH}/../mnt/usr/bin/
PREFIX_PATH=${PRJ_PATH}/install/bin

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

# Download source code packet
if [ ! -s $APP_NAME.$PACK_SUFIX ] ; then
   echo "+------------------------------------------------------------------+"
   echo "|  Download $APP_NAME.$PACK_SUFIX  now "  
   echo "+------------------------------------------------------------------+"

   wget $DL_ADDR
fi

if [ ! -d $PREFIX_PATH ] ; then
    mkdir -p $PREFIX_PATH
else
    echo "$APP_NAME already compile and installed, exit now..."
    exit
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
   line=`sed -n '/^CC=/=' $FILE`
   sed -i -e ${line}s"|.*|CC=${CROSS}gcc |" $FILE

   line=`sed -n '/^CFLAGS=-ggdb/=' $FILE`
   sed -i -e ${line}s"|.*|CFLAGS=-Wall -DLINUX|" $FILE
   sed -n "`expr $line + 1`p" $FILE | grep "XOBJS=strverscmp"
   if [ 0 != $? ] ; then
       sed -i -e ${line}a"XOBJS=strverscmp.o" $FILE
   fi

   make clean
   make
   ${CROSS}strip tree
   #sudo cp tree $INST_PATH
   sudo cp tree $PREFIX_PATH
cd -

