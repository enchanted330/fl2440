#!/bin/sh
# Description:  This shell script used to generate the patch file
#      Author:  GuoWenxue<guowenxue@gmail.com>
#    Changlog:
#         1,    Version 1.0.0(2011.04.01), initialize first version 
#               

PWD=`pwd`
PACKET_DIR=$PWD/packet/

# Parameter valid check
if [ $# != 2 ] ; then
    echo "+---------------------------------------------------"
    echo "|   Usage:  $0 [SRC_FOLDER] [BOARD]"
    echo "| Example:  $0 linux-3.4 stac"
    echo "+---------------------------------------------------"
    exit;
fi

SRC_NAME=MT7601U_LinuxAP_3.0.0.1_20130802
BOARD=$2
CPU=sam9g20

# Check latest source code exist or not
if [ ! -d $SRC_NAME ] ; then
    echo "+-------------------------------------------------------------------"
    echo "|  ERROR: Source code \"$SRC_NAME\" doesn't exist!"
    echo "+-------------------------------------------------------------------"
    exit;
fi

SRC_PACKET_PATH=$PACKET_DIR/$SRC_NAME.tar.bz2
# Check original source code packet exist or not
if [ ! -s $SRC_PACKET_PATH ] ; then
    echo "+-------------------------------------------------------------------"
    echo "| ERROR:  Orignal source code packet doesn't exist!"
    echo "| $SRC_PACKET_PATH"
    echo "+-------------------------------------------------------------------"
    exit;
fi

echo "+----------------------------------------------------------"
echo "|            Clean up the new source code                  "
echo "+----------------------------------------------------------"
NEW_SRC=$SRC_NAME-$BOARD
cd $SRC_NAME
make clean
cd ..
mv $SRC_NAME $NEW_SRC


echo "+------------------------------------------------------------------------"
echo "|           Decrompress orignal source code packet                       "
echo "+------------------------------------------------------------------------"
ORIG_SRC=$SRC_NAME
tar -xjf $SRC_PACKET_PATH

echo "+------------------------------------------------------------------------"
echo "|            Generate patch file \"$NEW_SRC.patch\"                      "
echo "+------------------------------------------------------------------------"

diff -Nuar $ORIG_SRC $NEW_SRC > $NEW_SRC.patch
rm -rf $ORIG_SRC
mv $NEW_SRC $SRC_NAME


