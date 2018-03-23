#!/bin/bash
INST_PATH=mnt
#INST_PATH=tmp

CROSSTOOL_PATH=/opt/buildroot-2012.08/arm920t/usr/
CMD_PREFIX=sudo

set -ev

LIB_PATH=$INST_PATH/lib
USR_LIB_PATH=$INST_PATH/usr/lib

$CMD_PREFIX rm -rf $LIB_PATH/*.so*
$CMD_PREFIX rm -rf $USR_LIB_PATH/*

$CMD_PREFIX cp -af $CROSSTOOL_PATH/arm-unknown-linux-uclibcgnueabi/sysroot/lib/*.so* $LIB_PATH
$CMD_PREFIX cp -af $CROSSTOOL_PATH/arm-unknown-linux-uclibcgnueabi/lib/*.so* $LIB_PATH
$CMD_PREFIX cp -af $CROSSTOOL_PATH/lib/*.so* $LIB_PATH
#$CMD_PREFIX sudo rm -rf $LIB_PATH/libmudflap*
$CMD_PREFIX sudo rm -rf $LIB_PATH/libstdc++.so.6.0.14-gdb.py

mkdir -p lib
$CMD_PREFIX cp -af $CROSSTOOL_PATH/arm-unknown-linux-uclibcgnueabi/sysroot/usr/lib/*.so* lib
#$CMD_PREFIX cp -af $CROSSTOOL_PATH/arm-unknown-linux-uclibcgnueabi/sysroot/usr/lib/engines lib
$CMD_PREFIX sudo rm -rf lib/libstdc++.so*
$CMD_PREFIX mv lib/* $USR_LIB_PATH 
rm -rf lib

