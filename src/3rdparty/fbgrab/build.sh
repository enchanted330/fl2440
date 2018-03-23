#!/bin/sh

#+--------------------------------------------------------------------------------------------
#|Description:  This shell script used to download file source code and cross compile it.
#|     Author:  GuoWenxue <guowenxue@gmail.com>
#|  ChangeLog:
#|           1, Initialize 1.0.0 on 2011.04.21
#+--------------------------------------------------------------------------------------------

PRJ_PATH=`pwd`

# CentOS 6.0 use file-5.04, so we should cross compile 5.04 for ARM too, for make magic file 
# depend on it.
APP_NAME="fbgrab-1.3"
PACK_SUFIX="tar.gz"
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
   export CC=${CROSS}gcc && export LDFLAGS=-static && make
   ${CROSS}strip fbgrab
   cp fbgrab $PREFIX_PATH
cd -

