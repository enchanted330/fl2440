#!/bin/sh
# Descripion:  This shell script used to choose a linux kernel version to cross compile
#     Author:  GuoWenxue<guowenxue@gmail.com>
#  ChangeLog:
#       1, Version 1.0.0(2011.04.15), initialize first version 
#

PWD=`pwd`
PACKET_DIR=$PWD/packet/
TOP_PRJ_SRC=$PWD/../
PATCH_DIR=$PWD/patch/

MT7601_DRIVER_DIR=MT7601U_LinuxAP_3.0.0.1_20130802
PATCH_FILE=${MT7601_DRIVER_DIR}-fl2440.patch


if [ ! -d $MT7601_DRIVER_DIR ] ; then
    tar -xjf $PACKET_DIR/MT7601U_LinuxAP_3.0.0.1_20130802.tar.bz2
fi

cd $MT7601_DRIVER_DIR
  patch -p1 < $PATCH_DIR/$PATCH_FILE
  make 
cd -

