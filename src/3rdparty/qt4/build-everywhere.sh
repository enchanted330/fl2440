#!/bin/sh

#+--------------------------------------------------------------------------------------------
#|Description:  This shell script used to download qt-extended-4.4.3 code and cross compile it.
#|     Author:  GuoWenxue <guowenxue@gmail.com>
#|  ChangeLog:
#|           1, Initialize 1.0.0 on 2012.03.28
#+--------------------------------------------------------------------------------------------

APP_NAME="qt-everywhere-opensource-src-4.6.4"
#APP_NAME="qt-everywhere-opensource-src-4.8.2"
PACK_SUFIX="tar.gz"
DL_ADDR="http://releases.qt-project.org/qt4/source/$APP_NAME.$PACK_SUFIX"

INST_PATH=/apps
LIBS_INST_PATH=$INST_PATH/libs
QT_INST_PATH=$INST_PATH/qt-everywhere

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


if [ -z "$CROSS" ] ; then
    if [ -z $ARCH ] ; then
        select_arch
    fi
    CROSS="/opt/buildroot-2012.08/${ARCH}/usr/bin/arm-linux-"
    #CROSS="/opt/buildroot-2012.05/${ARCH}/usr/bin/arm-linux-"
    #CROSS="/opt/crosstool/arm-arm920t-linux-gnueabi/bin/arm-${ARCH}-linux-gnueabi-"
fi

export CC=${CROSS}gcc 
export CXX=${CROSS}g++ 
export AR=${CROSS}ar 
export AS=${CROSS}as 
export LD=${CROSS}ld 
export NM=${CROSS}nm 
export RANLIB=${CROSS}ranlib 
export STRIP=${CROSS}strip
export PATH=`dirname $CROSS`:$PATH

mkdir -p ${LIBS_INST_PATH}
mkdir -p ${QT_INST_PATH}

if [ ! -f ${LIBS_INST_PATH}/lib/libts.so ]; then
    
    echo "+------------------------------------------------------------------+"
    echo "|          Build tslib for $ARCH "
    echo "| Crosstool:  $CROSS"
    echo "+------------------------------------------------------------------+"

    if [ ! -d tslib ] ; then
        git clone https://github.com/kergoth/tslib.git
    fi

    cd tslib

    rm -f mytslib.cache
    sh autogen.sh
    echo "ac_cv_func_malloc_0_nonnull=yes" >mytslib.cache

    ./configure --host=arm-linux --prefix=${LIBS_INST_PATH} --disable-linear-h2200 --disable-ucb1x00 --disable-corgi --disable-collie --disable-h3600 --disable-mk712 --disable-arctic2 --disable-linear-h2200 --cache-file=mytslib.cache CC=${CROSS}gcc AR=${CROSS}ar LD=${CROSS}ld 

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
    tar -xzf $APP_NAME.$PACK_SUFIX 
fi

cd $APP_NAME

#Moidfy the source code, fixed the uClibc in cross compiler not support pthread_getattr_np() bug
FILE=src/3rdparty/webkit/JavaScriptCore/runtime/Collector.cpp
if test -f $FILE  &&  echo $CROSS | grep "buildroot-2011.11" ; then 
    line=`sed -n '/__UCLIBC_SUBLEVEL__/=' $FILE`
    sed -i -e ${line}s"/.*/\t(__UCLIBC_MINOR__ == 9 \&\& __UCLIBC_SUBLEVEL__ <= 32)) \&\& defined(__LINUXTHREADS_OLD__)/" $FILE 
fi

FILE=mkspecs/qws/linux-arm-g++/qmake.conf
line=`sed -n '/guowenxue/=' $FILE`
if [ ! $line ] ; then
    line=`sed -n '/QMAKE_STRIP/=' $FILE` 
    sed -i -e `expr $line`a"## Add the additional libraries by guowenxue " $FILE
    sed -i -e `expr $line + 1`a"QMAKE_INCDIR += ${LIBS_INST_PATH}/include" $FILE
    sed -i -e `expr $line + 2`a"QMAKE_LIBDIR += ${LIBS_INST_PATH}/lib" $FILE
    sed -i -e `expr $line + 3`a"QMAKE_LFLAGS_RELEASE += -lts" $FILE
fi

#Configure for the QT
./configure -opensource -confirm-license -release -prefix ${QT_INST_PATH} \
-embedded arm -xplatform qws/linux-arm-g++ -shared -no-fast -no-largefile -no-multimedia \
-no-audio-backend  -no-phonon -no-phonon-backend -no-svg -no-opengl -qt-freetype \
-depths 16,18,24 -little-endian -qt-mouse-tslib -nomake tools -nomake docs -no-xmlpatterns \
-no-javascript-jit -no-script -no-scripttools -no-declarative -qt-zlib \
-no-gif -qt-libtiff -qt-libpng  -no-libmng -qt-libjpeg -no-openssl \
-no-nis -no-cups -no-dbus -no-mmx -no-3dnow -no-sse -no-sse2 -no-rpath -no-glib \
-no-xcursor -no-xfixes -no-xrandr -no-xrender -no-separate-debug-info

make -j 16 && make install

cd - 
