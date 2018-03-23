#!/bin/sh

#+--------------------------------------------------------------------------------------------
#|Description:  This shell script used to download tree wireless_tools code and cross compile it.
#|     Author:  GuoWenxue <guowenxue@gmail.com>
#|  ChangeLog:
#|           1, Initialize 1.0.0 on 2011.12.23
#+--------------------------------------------------------------------------------------------

PRJ_PATH=`pwd`

APP_NAME="wireless_tools.29"
PACK_SUFIX="tar.gz"
DL_ADDR="http://www.hpl.hp.com/personal/Jean_Tourrilhes/Linux/$APP_NAME.$PACK_SUFIX"
INST_PATH=${PRJ_PATH}/../mnt/usr/sbin/

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

if [ "arm920t" != $ARCH  -a "arm926t" != $ARCH ] ; then 
    echo "+------------------------------------------------------------------+"
    echo "|  ERROR: Unsupport platform $ARCH to cross compile "  
    echo "+------------------------------------------------------------------+"
    exit -1;
else
    CROSS="/opt/buildroot-2012.08/${ARCH}/usr/bin/arm-linux-"
fi

# Download source code packet
if [ ! -s $APP_NAME.$PACK_SUFIX ] ; then
    echo "+------------------------------------------------------------------+"
    echo "|  Download $APP_NAME.$PACK_SUFIX now "  
    echo "+------------------------------------------------------------------+" 
    wget $DL_ADDR 
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
   FILE=Makefile
   line=`sed -n '/^CC =/=' $FILE`
   sed -n "$line p" $FILE | grep "$CROSS"
   if [ 0 != $? ] ; then
       sed -i -e ${line}s"|.*|CC = ${CROSS}gcc|" $FILE

       line=`sed -n '/^AR =/=' $FILE`
       sed -i -e ${line}s"|.*|AR = ${CROSS}ar|" $FILE

       line=`sed -n '/^RANLIB =/=' $FILE`
       sed -i -e ${line}s"|.*|RANLIB = ${CROSS}ranlib|" $FILE

       line=`sed -n '/BUILD_STATIC =/=' $FILE`
       sed -i -e ${line}s"|.*|BUILD_STATIC = y|" $FILE
   fi

   make clean
   make
cd -

