#!/bin/sh
# Descripion:  This shell script used to choose a u-boot version to cross compile
#     Author:  GuoWenxue<guowenxue@gmail.com>
#  ChangeLog:
#       1, Version 1.0.0(2011.04.01), initialize first version 
#       2, Version 1.0.1(2011.04.03), modify it to compatible with Linux kernel build script
#       3, Version 1.1.0(2011.04.10), modify to support s3c2410 cross compile support
#

#User can pass a argument to specify which version should be cross compile
#or uncomment the SRC_NAME variable to specify the version 

#SRC_NAME=u-boot-2010.09

PWD=`pwd`
PACKET_DIR=$PWD
PATCH_DIR=$PWD/patch
INST_PATH=$PWD/../../../bin

BOARD=fl2440

if [ -z "$CROSS" ] ; then
    CROSS=/opt/buildroot-2012.08/arm920t/usr/bin/arm-linux-
fi

#===============================================================
#               Functions forward definition                   =
#===============================================================
sup_ver=("" "u-boot-2010.09")
function select_version()
{
   echo "Current support U-Boot version:"
   i=1
   len=${#sup_ver[*]}

   while [ $i -lt $len ]; do
       echo "$i: ${sup_ver[$i]}"
       let i++;
   done

   if [ $len -eq 2 ] ; then
       SRC_NAME=${sup_ver[1]}
       return;
   fi

   echo "Please select: "
   index=
   read index 

   SRC_NAME=${sup_ver[$index]}
}

sup_boards=("" "fl2440")
function select_board()
{
   echo "Current support S3C24X0 boards:"
   i=1
   len=${#sup_boards[*]}


   while [ $i -lt $len ]; do
       echo "$i: ${sup_boards[$i]}"
       let i++;
   done

   echo "Please select: "
   index=
   read index

   BOARD=${sup_boards[$index]}
}


function disp_banner()
{
   echo ""
   echo "*******************************************************"
   echo "*     Cross compile $SRC_NAME for $BOARD now...       "
   echo "*******************************************************"
   echo ""
}

#===============================================================
#                   Script excute body start                   =
#===============================================================

# If not define default version, then let user choose a one
if [ -z $SRC_NAME ] ; then
    select_version
fi

# If don't set the BOARD, then select one
if [ -z $BOARD ] ; then
   select_board
fi

disp_banner

# If $SRC_NAME not set, then abort this cross compile
if [ -z $SRC_NAME ] ; then 
    echo "ERROR: Please choose a valid version to cross compile"
    exit 1;
fi

# Check patche file exist or not
PATCH_FILE=$PATCH_DIR/$SRC_NAME-$BOARD.patch
if [ ! -f $PATCH_FILE ] ; then
    echo "ERROR:$SRC_NAME patch file doesn't exist:"
    echo "PATH: \"$PATCH_FILE\""
    echo ""
    exit
fi

# Check original source code packet exist or not
SRC_ORIG_PACKET=$PACKET_DIR/$SRC_NAME.tar.bz2
if [ ! -s $SRC_ORIG_PACKET ] ; then
    echo "============================================================================"
    echo "ERROR:$SRC_NAME source code patcket doesn't exist:"
    echo "PATH: \"$SRC_ORIG_PACKET\""
    DL_ADDR="ftp://ftp.denx.de/pub/u-boot/$SRC_NAME.tar.bz2"
    echo ""
    echo "Download $DL_ADDR now... "
    echo "============================================================================"
    wget $DL_ADDR
    
    if [ ! -s $SRC_ORIG_PACKET ] ; then
        echo "Download $DL_ADDR failure, exit now... "
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

#If don't do "make $BOARD_config", it will configure for the board first
if [ ! -s include/config.mk ] ; then
    make ${BOARD}_config
fi
make

CPU="s3c`echo $BOARD | sed 's/[^0-9.]//g'`"

set -x
cp -af u-boot-$CPU.bin /tftp

