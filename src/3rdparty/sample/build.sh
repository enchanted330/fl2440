#!/bin/sh

#+--------------------------------------------------------------------------------------------
#|Description:  This is the sample shell script to show hostw to download a third party application
#|             and cross compile it.
#|     Author:  GuoWenxue <guowenxue@gmail.com>
#|  ChangeLog:
#|           1, Initialize 1.0.0 on 2011.12.26
#+--------------------------------------------------------------------------------------------

PRJ_PATH=`pwd`

APP_NAME="sample_code"
APP_PACK=$APP_NAME.tar.gz

INST_PATH=$PRJ_PATH/install

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

# arg1 is the packet download address
# arg2 is the download packet rename name
function download_packet()
{
    if [ $# -lt 1 ] ; then
        echo "ERROR: $0 without argument for packet download address"
        exit;
    fi

    rename=0

    dl_addr=$1
    dl_file=`basename $dl_addr`

    if [ $# -ge 2 ] ; then
        pack=$2
        rename=1
    else
        pack=$dl_file
    fi

    if [ -s $pack ] ; then
        echo "INFO: $pack already exist, skip download it"
        return 0;
    fi

    echo "INFO: Start to download packet $pack now"
    wget $dl_addr 

    if [ $? != 0  ] ; then
        echo "ERROR: Download $pack failure, exit now..."
        exit;
    fi 
    echo "INFO: Download $pack ok!"

    if [ $rename != 0 ] ; then
        mv  $dl_file $pack
    fi

    return 0;
}

function decompress_packet()
(
   echo "+---------------------------------------------+"
   echo "|  Decompress $1 now"  
   echo "+---------------------------------------------+"
   rv=0;

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
           rv=1;
           echo "$1 is unknow compress format";;
    esac

    if [ $rv != 0 ] ; then
        echo "ERROR: Decompress $1 failure, exit now..."
        exit 1;
    fi

    return $rv;
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

if [ -d $INST_PATH ] ; then
    echo "INFO: $APP_NAME already cross compiled, exit now ..."
    exit;
fi

# Download source code packet
download_packet http://www.domain.com/files/sample_code.tar.gz

# Decompress source code packet 
if [ ! -d $APP_NAME ] ; then 
    decompress_packet $APP_PACK
    if [ $? != 0 ] ; then
        exit 1;
    fi
fi


echo "+------------------------------------------------------------------+"
echo "|          Build $APP_NAME for $ARCH "
echo "| Crosstool:  $CROSS"
echo "+------------------------------------------------------------------+"

cd $APP_NAME
   set -x
   LDFLAGS=-static ./configure --host=arm-linux --prefix=$INST_PATH
   make && make install
cd -

