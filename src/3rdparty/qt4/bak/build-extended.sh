#!/bin/sh

#+--------------------------------------------------------------------------------------------
#|Description:  This shell script used to download qt-extended-4.4.3 code and cross compile it.
#|     Author:  GuoWenxue <guowenxue@gmail.com>
#|  ChangeLog:
#|           1, Initialize 1.0.0 on 2012.03.28
#|
#|   Build QT-extended-4.4.3 depend on follow system library: libX11-devel, libXext-devel
#|     [guowenxue@centos6 qt4]$ sudo yum install -y libXext-devel  
#+--------------------------------------------------------------------------------------------

PRJ_PATH=`pwd`

APP_NAME="qt-extended-opensource-src-4.4.3"
PACK_SUFIX="tar.gz"
#Official web site: https://sourceforge.net/projects/qpe/files/QPE/qtopia/
DL_ADDR="http://nchc.dl.sourceforge.net/project/qpe/QPE/qtopia/$APP_NAME.$PACK_SUFIX"
INST_PATH=/apps/

ARCH=arm920t

if [ -z "$ARCH" -a $# -gt 0 ] ; then
   ARCH=$1
fi

sup_arch=("" "arm920t" )

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


if [ -z "$CROSS" ] ; then
    if [ -z $ARCH ] ; then
        select_arch
    fi
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

if [ ! -d $INST_PATH ] ; then
    mkdir -p $INST_PATH
fi

echo "+------------------------------------------------------------------+"
echo "|          Build tslib for $ARCH "
echo "| Crosstool:  $CROSS"
echo "+------------------------------------------------------------------+"

if [ ! -d ${INST_PATH}/tslib ]; then
    if [ ! -d tslib ] ; then
        git clone https://github.com/kergoth/tslib.git
    fi

    cd tslib

    rm -f mytslib.cache
    sh autogen.sh
    echo "ac_cv_func_malloc_0_nonnull=yes" >mytslib.cache

    ./configure --host=arm-linux --prefix=${INST_PATH}/tslib --disable-linear-h2200 --disable-ucb1x00 --disable-corgi --disable-collie --disable-h3600 --disable-mk712 --disable-arctic2 --disable-linear-h2200 --cache-file=mytslib.cache CC=${CROSS}gcc AR=${CROSS}ar LD=${CROSS}ld 

    make && make install

    cd -
fi

unset CC 
unset CXX 
unset AR
unset AS
unset LD
unset NM
unset RANLIB
unset STRIP
export PATH=`dirname $CROSS`:$PATH

echo "+------------------------------------------------------------------+"
echo "|          Build $APP_NAME for $ARCH "
echo "| Crosstool:  $CROSS"
echo "+------------------------------------------------------------------+"

# Download source code packet
if [ ! -s $APP_NAME.$PACK_SUFIX ] ; then 
    echo "+------------------------------------------------------------------+" 
    echo "|  Download $APP_NAME.$PACK_SUFIX now "  
    echo "+------------------------------------------------------------------+" 
    wget $DL_ADDR 
fi 

# Decompress source code packet 
if [ ! -d $APP_NAME ] ; then 
    #decompress_packet $APP_NAME.$PACK_SUFIX 
    tar -xzf $APP_NAME.$PACK_SUFIX 
fi

# We always don't cross compile QT in the source code folder
rm -rf mybuild
mkdir mybuild
cd mybuild

PWD=`pwd`
PRJ=`dirname ${PWD}`
export QTOPIA_DEPOT_PATH=$PRJ/qt-extended-4.4.3

#Moidfy the source code, fixed the uClibc in cross compiler not support pthread_getattr_np() bug
FILE=${QTOPIA_DEPOT_PATH}/qtopiacore/qt/src/3rdparty/webkit/JavaScriptCore/kjs/collector.cpp
line=`sed -n '/__UCLIBC_SUBLEVEL__/=' $FILE`
sed -i -e ${line}s"/.*/\t(__UCLIBC_MINOR__ == 9 \&\& __UCLIBC_SUBLEVEL__ <= 32)) \&\& defined(__LINUXTHREADS_OLD__)/" $FILE

#Configure for the QT
$QTOPIA_DEPOT_PATH/configure -little-endian -confirm-license -release \
-L${INST_ROOT}/tslib/lib -I${INST_ROOT}/tslib/include \
-no-rpath -no-glib \
-image ${INST_PATH}/qt-extended-4.4.3 -prefix ${INST_PATH}/qt-extended-4.4.3 \
-xplatform linux-arm-g++ -arch arm \
-remove-module bluetooth -remove-module cell \
-modules connectivity -modules devtools \
-modules drm -remove-module infrared \
-modules ipcomms -modules messaging \
-remove-module telephony -modules pkgmanagement \
-modules ui -ui-type home -modules essentials -modules games \
-modules inputmethods -add-module media -modules pim \
-no-ssl -no-dbus -no-dbusipc -no-v4l2 -no-vpn -no-libamr \
-displaysizes 240x320 -build-qt 
-extra-qt-embedded-config "-little-endian -xplatform qws/linux-arm-g++ -embedded arm -qconfig qpe -fast -no-largefile -no-pch -no-mmx -no-3dnow -no-sse -no-sse2 -shared -webkit -qt-libpng -qt-libjpeg -qt-gif -depths 4,8,16,32 -no-gfx-qvfb -no-kbd-qvfb -no-kbd-tty -no-mouse-linuxtp -no-mouse-pc -no-mouse-qvfb -qt-mouse-tslib"

#Start to cross compile QT
bin/qbuild && bin/qbuild image



