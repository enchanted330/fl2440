#!/bin/sh

#+--------------------------------------------------------------------------------------------
#|Description:  This shell script used to download net-snmp source code and cross compile it.
#|     Author:  GuoWenxue <guowenxue@gmail.com>
#|  ChangeLog:
#|           1, Initialize 1.0.0 on 2011.04.21
#+--------------------------------------------------------------------------------------------

PRJ_PATH=`pwd`

APP_NAME="net-snmp-5.4.4"
PACK_SUFIX="tar.gz"
DL_ADDR="http://matt.ucc.asn.au/dropbear/releases/$APP_NAME.$PACK_SUFIX"
INST_PATH=${PRJ_PATH}/../mnt/usr/sbin/
PREFIX_PATH=/apps/snmp/

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

# Download source code packet
if [ ! -s $APP_NAME.$PACK_SUFIX ] ; then
   echo "+------------------------------------------------------------------+"
   echo "|  Download $APP_NAME.$PACK_SUFIX now "  
   echo "+------------------------------------------------------------------+"

   wget $DL_ADDR
fi

rm -rf $PREFIX_PATH
if [ ! -d $PREFIX_PATH ] ; then 
    mkdir -p $PREFIX_PATH
else
    echo "$APP_NAME already compile and installed, exit now..."
    exit
fi

rm -rf $APP_NAME
# Decompress source code packet
if [ ! -d $APP_NAME ] ; then
    decompress_packet $APP_NAME.$PACK_SUFIX
fi


echo "+------------------------------------------------------------------+"
echo "|          Build $APP_NAME for $ARCH "
echo "| Crosstool:  $CROSS"
echo "+------------------------------------------------------------------+"

#export CC=${CROSS}gcc 
#export LD=${CROSS}ld 
#export AS=${CROSS}as 
#export CXX=${CROSS}g++ 
#export AR=${CROSS}ar 
#export NM=${CROSS}nm 
export STRIP=${CROSS}strip 
#export OBJCOPY=${CROSS}objcopy 
#export OBJDUMP=${CROSS}objdump 
#export RANLIB=${CROSS}ranlib
#export CFLAGS+=-DHAVE_STRERROR

cd $APP_NAME
   CC=${CROSS}gcc LD=${CROSS}ld AS=${CROSS}as CXX=${CROSS}g++ AR=${CROSS}ar NM=${CROSS}nm RANLIB=${CROSS}ranlib \
   ./configure --prefix=$PREFIX_PATH --build=i686 --host=arm-linux --with-endianness=little \
<<<<<<< .mine
   --with-cc=${CROSS}gcc --with-linkcc=${CROSS}gcc --disable-ucd-snmp-compatibility --with-mibdirs="/apps/snmp/mibs" \
   --enable-static --disable-shared --disable-manuals  --enable-mibs --disable-embedded-perl \
=======
   --with-cc=${CROSS}gcc --with-linkcc=${CROSS}gcc --disable-ucd-snmp-compatibility --with-mibdirs="/apps/snmp/mibs" \
   --disable-ipv6 --enable-static --disable-shared --disable-manuals  --enable-mibs --disable-embedded-perl \
>>>>>>> .r154
   --without-perl-modules --disable-snmptrapd-subagent --disable-scripts
   make && make install
   $STRIP $PREFIX_PATH/bin/* $PREFIX_PATH/sbin/*

cd -

