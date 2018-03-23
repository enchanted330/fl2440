#!/bin/sh

#+--------------------------------------------------------------------------------------------
#|Description:  This shell script used to cross compile all the apps in current folder.
#|     Author:  GuoWenxue <guowenxue@gmail.com>
#|  ChangeLog:
#|           1, Initialize 1.0.0 on 2011.04.21
#+--------------------------------------------------------------------------------------------

PRJ_PATH=`pwd`
ARCH=arm920t

sup_arch=("" "arm920t" "arm926t")
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

if [ -z "$CROSS" ] ; then
   if [ -z "$ARCH" -a $# -gt 0 ] ; then 
     ARCH=$1
   fi
   if [ -z "$ARCH" ] ; then
      select_arch
   fi
   CROSS="/opt/buildroot-2012.08/${ARCH}/usr/bin/arm-linux-"
fi

INST_ROOT=$PRJ_PATH/$ARCH
APPS_ROOT=$PRJ_PATH/apps
#INST_ROOT=$PRJ_PATH/../rootfs/mnt/

if [ ! -d  $INST_ROOT ] ; then
    mkdir -p $INST_ROOT/{bin,sbin,usr/{bin,sbin,lib}}
    mkdir -p $APPS_ROOT/{tools,etc,lib}
fi

export ARCH
export CROSS
export INST_ROOT
export APPS_ROOT 

for i in `ls | grep -v "etc" | grep -v "ldattach"`; do
        if [ -f $i/build.sh ] ; then 
            cd $i 
            sh build.sh $ARCH 
            cd -
        elif [ -f $i/makefile -o -f $i/Makefile ] ; then
            cd $i
            make && make install
            cd -
        fi
done
