#!/bin/sh
# Descripion:  This shell script used to choose a linux kernel version to cross compile
#     Author:  GuoWenxue<guowenxue@gmail.com>
#  ChangeLog:
#       1, Version 1.0.0(2011.04.01), initialize first version 
#

PWD=`pwd`
PACKET_DIR=$PWD
PATCH_DIR=$PWD/patch
INST_PATH=$PWD/../../../bin
SRC_NAME=linux-3.0
ARCH=s3c2440
#unset ARCH

#===============================================================
#               Functions forward definition                   =
#===============================================================
function disp_banner()
{
   echo ""
   echo "+------------------------------------------+"
   echo "|      Build $SRC_NAME for $ARCH            "
   echo "+------------------------------------------+"
   echo ""
}

sup_ver=("" "linux-3.0" "linux-3.1" "linux-3.2" "linux-3.3" "linux-3.4" "linux-2.6.33" "linux-2.6.38" "linux-2.6.24")
function select_version()
{
   echo "Current support linux kernel version:"
   i=1
   len=${#sup_ver[*]} 

   while [ $i -lt $len ]; do
       echo "$i: ${sup_ver[$i]}"
       let i++;
   done

   echo "Please select: "
   index=
   read index 

   SRC_NAME=${sup_ver[$index]}
}

sup_arch=("" "s3c2440" "s3c2410")
function select_arch()
{
   echo "Current support S3C24X0 ARCH:"
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

#===============================================================
#                   Script excute body start                   =
#===============================================================

# If not define default version, then let user choose a one
if [ -z $SRC_NAME ] ; then
    select_version
fi

# If don't set the ARCH, then select one
if [ -z $ARCH ] ; then
   select_arch
fi


if [ -z "$CROSS" ] ; then
    CROSS=/opt/buildroot-2012.08/arm920t/usr/bin/arm-linux-
fi

# If $SRC_NAME not set, then abort this cross compile
if [ -z $SRC_NAME ] ; then 
    echo "ERROR: Please choose a valid version to cross compile"
    exit 1;
fi

disp_banner    #Display this shell script banner


# Check patche file exist or not
PATCH_FILE=$PATCH_DIR/$SRC_NAME-${ARCH}.patch
if [ ! -f $PATCH_FILE ] ; then
    echo "ERROR:$SRC_NAME patch file doesn't exist!"
    echo "PATH: \"$PATCH_FILE\"" 
    exit
fi

# Check original source code packet exist or not
SRC_ORIG_PACKET=$PACKET_DIR/$SRC_NAME.tar.bz2
if [ ! -s $SRC_ORIG_PACKET ] ; then
    echo "========================================================================="
    echo "ERROR:$SRC_NAME source code patcket doesn't exist."
    echo "PATH: \"$SRC_ORIG_PACKET\""
    echo ""
    
    MAJOR=`echo $SRC_NAME | awk -F "-" '{print $2}' | awk -F "." '{print $1}'`
    if [ $MAJOR == 3 ] ; then
        DL_ADDR=http://www.kernel.org/pub/linux/kernel/v3.x/$SRC_NAME.tar.bz2
    elif [ $MAJOR == 2 ] ; then
        DL_ADDR=http://www.kernel.org/pub/linux/kernel/v2.6/$SRC_NAME.tar.bz2
    fi

    echo "Download $DL_ADDR now..."
    echo "========================================================================="

    wget $DL_ADDR

    if [ ! -s $SRC_ORIG_PACKET ] ; then
        echo "Download $DL_ADDR failure, exit now..."
        exit
    fi
fi


#decompress the source code packet and patch
echo "*  Decompress the source code patcket and patch now...  *"

if [ -d $SRC_NAME ] ; then
    rm -rf $SRC_NAME
fi

if [ ! -d $INST_PATH ] ; then
    mkdir -p $INST_PATH
fi

#Remove old source code
tar -xjf $SRC_ORIG_PACKET

#Start to cross compile the source code and install it now
cd $SRC_NAME
   patch -p1 < $PATCH_FILE
   cp .cfg-$ARCH .config
   #Modify the cross tool in Makefile
   line=`sed -n '/CROSS_COMPILE	?= /=' Makefile`
   sed -i -e ${line}s"|.*|CROSS_COMPILE	?= $CROSS|" Makefile
   make

   VERSION=`echo $SRC_NAME | awk -F "-" '{print $2}'`
   set -x
   cp -af linuxrom-$ARCH.bin $INST_PATH/
   cp -af linuxrom-$ARCH.bin /tftp/
cd -


