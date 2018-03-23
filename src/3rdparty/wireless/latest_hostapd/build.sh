#!/bin/sh

#+--------------------------------------------------------------------------------------------
#|Description:  This shell script used to download tree wireless_tools code and cross compile it.
#|     Author:  GuoWenxue <guowenxue@gmail.com>
#|  ChangeLog:
#|           1, Initialize 1.0.0 on 2011.12.26
#+--------------------------------------------------------------------------------------------

PRJ_PATH=`pwd`
PRJ_TOP=${PRJ_PATH}/..

#APP_NAME="hostapd-0.7.3"
#PACK_SUFIX="tar.gz"
APP_NAME="hostap"
PACK_SUFIX="tar.bz2"
DL_ADDR="http://hostap.epitest.fi/releases/$APP_NAME.$PACK_SUFIX"
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

# Download source code packet if it doesn't exist, or decompress it
if [ ! -d $APP_NAME ] ; then
    if [ -s $APP_NAME.$PACK_SUFIX ] ; then
        decompress_packet $APP_NAME.$PACK_SUFIX
    else
        git clone git://w1.fi/srv/git/hostap.git
    fi
fi

echo "+------------------------------------------------------------------+"
echo "|          Build $APP_NAME for $ARCH "
echo "| Crosstool:  $CROSS"
echo "+------------------------------------------------------------------+"

OPENSSL_PATH=$PRJ_TOP/openssl
if [ ! -d $OPENSSL_PATH/install/lib ]; then
    echo "OpenSSL not cross compiled, goto cross compile it first."
    cd $OPENSSL_PATH
    sh build.sh
    cd -
fi

LIBNL_PATH=$PRJ_TOP/libnl
if [ ! -d $LIBNL_PATH/install/lib ]; then
    echo "Netlink library libnl not cross compiled, goto cross compile it first."
    cd $LIBNL_PATH
    sh build.sh
    cd -
fi

#Cross compile hostapd now
cd $APP_NAME/hostapd
   cp defconfig .config
   FILE=.config

   line=`sed -n '/CONFIG_DRIVER_HOSTAP=/=' $FILE` 
   if [ -n "$line" ] ; then
       sed -i -e `expr $line + 1`a"CC=${CROSS}gcc" $FILE
       sed -i -e `expr $line + 2`a"CFLAGS+=-I${OPENSSL_PATH}/install/include" $FILE
       sed -i -e `expr $line + 3`a"LDFLAGS+=-L${OPENSSL_PATH}/install/lib -lssl\n" $FILE
   fi

   line=`sed -n '/CONFIG_DRIVER_NL80211=/=' $FILE` 
   if [ -n "$line" ] ; then
       sed -i -e ${line}s"|.*|CONFIG_DRIVER_NL80211=y|" $FILE
       sed -i -e `expr $line + 0`a"LIBNL=$PRJ_TOP/libnl/install" $FILE
       sed -i -e `expr $line + 1`a"CFLAGS += -I\$(LIBNL)/include/" $FILE
       sed -i -e `expr $line + 2`a"LIBS += -L\$(LIBNL)/lib" $FILE
   fi

   sed -i -e "s/CONFIG_WPS=/CONFIG_WPS=y/g" $FILE
   sed -i -e "s/CONFIG_PKCS12=/#CONFIG_PKCS12=y/g" $FILE
   sed -i -e "s/CONFIG_IPV6=/#CONFIG_IPV6=y/g" $FILE
   sed -i -e "s/CONFIG_IEEE80211N=/CONFIG_IEEE80211N=y/g" $FILE

   make
   set -x
   ${CROSS}strip hostapd
cd -



#Cross compile wpa_supplicant now
cd $APP_NAME/wpa_supplicant
   cp defconfig .config
   FILE=.config
   
   line=`sed -n '/CONFIG_DRIVER_HOSTAP=/=' $FILE`
   if [ -n "$line" ] ; then
       sed -i -e `expr $line + 1`a"CC=${CROSS}gcc" $FILE
       sed -i -e `expr $line + 2`a"CFLAGS+=-I${OPENSSL_PATH}/install/include" $FILE
       sed -i -e `expr $line + 3`a"LDFLAGS+=-L${OPENSSL_PATH}/install/lib -lssl\n" $FILE
   fi

   sed -i -e "s/CONFIG_DRIVER_ATMEL=/#CONFIG_DRIVER_ATMEL=y/g" $FILE
   sed -i -e "s/CONFIG_DRIVER_RALINK=/CONFIG_DRIVER_RALINK=y/g" $FILE
   sed -i -e "s/CONFIG_PKCS12=/#CONFIG_PKCS12=y/g" $FILE
   sed -i -e "s/CONFIG_SMARTCARD=/#CONFIG_SMARTCARD=y/g" $FILE

   line=`sed -n '/CONFIG_DRIVER_NL80211=/=' $FILE`
   if [ -n "$line" ] ; then
       sed -i -e ${line}s"|.*|CONFIG_DRIVER_NL80211=y|" $FILE
       sed -i -e `expr $line + 0`a"LIBNL=$PRJ_TOP/libnl/install" $FILE
       sed -i -e `expr $line + 1`a"CFLAGS += -I\$(LIBNL)/include/" $FILE
       sed -i -e `expr $line + 2`a"LIBS += -L\$(LIBNL)/lib" $FILE
       sed -i -e `expr $line + 4`a"CONFIG_IEEE80211N=y\n" $FILE
   fi 
   
   make
   set -x
   ${CROSS}strip wpa_supplicant
cd -
