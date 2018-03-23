#!/bin/sh

#+--------------------------------------------------------------------------------------------
#|Description:  This shell script used to download busybox source code and cross compile it.
#|     Author:  GuoWenxue <guowenxue@gmail.com>
#|  ChangeLog:
#|           1, Initialize 1.0.0 on 2011.04.21
#+--------------------------------------------------------------------------------------------

PRJ_PATH=`pwd`
ARCH=arm920t

APP_NAME="busybox-1.24.2"
PATCH_FILE=${PRJ_PATH}/patch/$APP_NAME.patch
PACK_SUFIX="tar.bz2"
DL_ADDR="http://www.busybox.net/downloads/${APP_NAME}.${PACK_SUFIX}"

if [ -z "$ARCH" -a $# -gt 0 ] ; then
   ARCH=$1
fi
#INST_PATH=../../../rootfs/rootfs_tree
INST_PATH=$PRJ_PATH/../../linux/rootfs/rootfs_tree

sup_arch=("" "arm920t" "arm926t" )
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

    ftype=`file -L "$1"`
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
  if [ -z "$ARCH" ] ; then
    select_arch
  fi
  CROSS="/opt/buildroot-2012.08/${ARCH}/usr/bin/arm-linux-"
fi


# Download source code packet
if [ ! -s $APP_NAME.$PACK_SUFIX ] ; then
   echo "+------------------------------------------------------------------+"
   echo "|  Download $APP_NAME.$PACK_SUFIX  now "  
   echo "+------------------------------------------------------------------+"

   wget $DL_ADDR
fi

# Decompress source code packet
if [ ! -d $APP_NAME ] ; then
    decompress_packet $APP_NAME.tar.*
fi

if [ -z $INST_PATH ] ; then
   INST_PATH=$PRJ_PATH/../$ARCH/mnt
fi

echo "+------------------------------------------------------------------+"
echo "|          Build $APP_NAME for $ARCH "
echo "| Crosstool:  $CROSS"
echo "+------------------------------------------------------------------+"

cd $APP_NAME
   if [ -s $PATCH_FILE ]; then
      patch -p1 < $PATCH_FILE
   fi

   set -x
   cp .cfg .config

   #Modify the cross config in the configure file
   sed -i -e "s|^CONFIG_CROSS_COMPILER_PREFIX=.*|CONFIG_CROSS_COMPILER_PREFIX=\"$CROSS\"|g" .config

   #Modify the install path in the configure file
   sed -i -e "s|^CONFIG_PREFIX=.*|CONFIG_PREFIX=\"$INST_PATH\"|g" .config

   make

   #install busybox
   if [ -d $INST_PATH ] ; then
     sudo rm -rf $INST_PATH/bin/*
     sudo rm -rf $INST_PATH/sbin/*
     sudo make install 
   fi
cd  -

