#!/bin/sh

#+--------------------------------------------------------------------------------------------
#|Description:  This shell script used to download dropbear source code and cross compile it.
#|     Author:  GuoWenxue <guowenxue@gmail.com>
#|  ChangeLog:
#|           1, Initialize 1.0.0 on 2011.04.21
#+--------------------------------------------------------------------------------------------

PRJ_PATH=`pwd`

APP_NAME="dhcp-4.0.3"
PACK_SUFIX="tar.gz"
DL_ADDR="ftp://ftp.isc.org/isc/dhcp/$APP_NAME.$PACK_SUFIX"
INST_PATH=${PRJ_PATH}/../mnt/usr/sbin/
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
CC=${CROSS}gcc
AR=${CROSS}ar
LD=${CROSS}ld
STRIP=${CROSS}strip
NM=${CROSS}nm
RANLIB=${CROSS}ranlib
OBJDUMP=${CROSS}objdump

export CC=${CC}
export AR=${AR}
export LD=${LD}
export STRIP=${STRIP}
export NM=${NM}
export RANLIB=${RANLIB}
export OBJDUMP=${OBJDUMP}

# Download source code packet
if [ ! -s $APP_NAME.$PACK_SUFIX ] ; then
   echo "+------------------------------------------------------------------+"
   echo "|  Download $APP_NAME.$PACK_SUFIX now "  
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
   echo "ac_cv_file__dev_random=yes" > arm-linux.cache 
   ./configure --prefix=$PREFIX_PATH CC=${CROSS}gcc --build=i686-pc-linux --host=arm-linux- \
   --cache-file=arm-linux.cache -with-srv-lease-file=/tmp/dhcpd.leases \
   --with-srv-pid-file=/var/run/dhcpd.pid --with-relay-pid-file=/var/run/dhcrelay.pid
   make

   ${CROSS}strip server/dhcpd
   make install
cd -

